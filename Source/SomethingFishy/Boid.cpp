// Fill out your copyright notice in the Description page of Project Settings.

#include "Boid.h"
#include "Flock.h"
#include "BaitManager.h"
#include "Bait.h"
#include "PlayerPawn.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DrawDebugHelpers.h"

/****************************************
   *  Basic Public Functions
*****************************************/
// Sets default values
ABoid::ABoid()
{
   // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
   PrimaryActorTick.bCanEverTick = true;

   // Set up visual mesh
   VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
   VisualMesh->SetupAttachment(RootComponent);

   static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/LPAnimals/Meshes/Fish.Fish"));

   if (SphereVisualAsset.Succeeded())
   {
      VisualMesh->SetStaticMesh(SphereVisualAsset.Object);
      VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
      VisualMesh->SetRelativeScale3D(FVector(0.4, 0.4, 0.4));
   }

   // Set up projectile component
   ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
   ProjectileMovementComponent->SetUpdatedComponent(VisualMesh);
   ProjectileMovementComponent->InitialSpeed = 0.0;
   ProjectileMovementComponent->bRotationFollowsVelocity = true;
   ProjectileMovementComponent->bShouldBounce = true;
   ProjectileMovementComponent->Bounciness = 0.3f;
   ProjectileMovementComponent->ProjectileGravityScale = 0;

   VisualMesh->SetCollisionProfileName(TEXT("Boids"));
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
   Super::Tick(DeltaTime);

   std::vector<ABoid*> curr_flockmates;
   myFlock->AFlock::GetVisibleFlockmates(this, curr_flockmates);

   // Make vector for current direction
   FVector moveDirection = GetActorRotation().Vector();

   // Calculate logic forces
   if (curr_flockmates.size())
   {
      moveDirection += Separation(curr_flockmates) * myFlock->separation_weight;
      moveDirection += Alignment(curr_flockmates) * myFlock->alignment_weight;
      moveDirection += Cohesion(curr_flockmates) * myFlock->cohesion_weight;
   }
   if (myFlock->baitManager && myFlock->baitManager->NotEmpty())
   {
      moveDirection += Target() * myFlock->target_weight;
   }
   moveDirection += Bounds() * myFlock->bounds_weight;
   moveDirection += AvoidObstacles() * myFlock->avoidObstacles_weight;
   moveDirection += AvoidPlayer() * myFlock->avoidPlayer_weight;

   moveDirection += Centralize() * myFlock->centralize_weight;

   // Temporary fix for occasional garbage moveDirection which propagates. fix later
   if (moveDirection.Size() < myFlock->speed * 100 && moveDirection.Size() > -myFlock->speed * 100)
   {
      ProjectileMovementComponent->Velocity += moveDirection * myFlock->speed * DeltaTime;
   }
   else
   {
      ProjectileMovementComponent->Velocity = GetActorRotation().Vector() * myFlock->speed * DeltaTime;
   }
   if (!ProjectileMovementComponent->Velocity.Size()) // bugged, reset
   {
      this->Enter();
   }
}

// Set the flock that boid is a member of
void ABoid::SetFlock(AFlock* flock)
{
   myFlock = flock;
}

// remove from world
void ABoid::Remove()
{
   myFlock->Remove(this);
}

// set rotation and position to enter/re-enter main play area
void ABoid::Enter()
{
   FVector loc(0, UKismetMathLibrary::RandomFloat() * myFlock->bounds.Y, 500);
   FRotator rot = (loc - myFlock->bounds / 2).Rotation();

   SetActorLocation(loc);
   SetActorRotation(rot);
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
   Super::BeginPlay();
   if (myFlock) 
   {
      ProjectileMovementComponent->MaxSpeed = myFlock->speed;
   }
   else
   {
      ProjectileMovementComponent->MaxSpeed = 600;
   }
}


/****************************************
   *  Behavior Functions
*****************************************/
// Separation: Steer to avoid crowding local flockmates
FVector ABoid::Separation(const std::vector<ABoid*>& flockmates)
{
   FVector force(0, 0, 0);
   FVector loc = GetActorLocation();
   for (unsigned int i = 0; i < flockmates.size(); ++i)
   {
      float dist = FVector::Dist(loc, flockmates[i]->GetActorLocation());
      FVector diff = loc - flockmates[i]->GetActorLocation();
      diff *= diff.Size();
      diff /= dist;
      force += diff;
   }

   force /= flockmates.size();
   force /= force.Size();
   force *= myFlock->speed;
   force -= ProjectileMovementComponent->Velocity;
   force.Normalize(myFlock->max_force);

   if (force.Size() < myFlock->speed * 100 && force.Size() > -myFlock->speed * 100)
   {
      return force;
   }
   else
   {
      return FVector(0, 0, 0);
   }
}

// Alignment: Steer towards the average heading of local flockmates
FVector ABoid::Alignment(const std::vector<ABoid*>& flockmates)
{
   FVector force(0, 0, 0);
   for (unsigned int i = 0; i < flockmates.size(); ++i)
   {
      force += flockmates[i]->GetVelocity();
   }

   force /= flockmates.size();
   force /= force.Size();
   force *= myFlock->speed;
   force -= ProjectileMovementComponent->Velocity;
   force.Normalize(myFlock->max_force);

   if (force.Size() < myFlock->speed * 100 && force.Size() > -myFlock->speed * 100)
   {
      return force;
   }
   else
   {
      return FVector(0, 0, 0);
   }
}

// Cohesion: Steer to move toward the average position of local flockmates
FVector ABoid::Cohesion(const std::vector<ABoid*>& flockmates)
{
   FVector position = FVector(0, 0, 0);
   for (unsigned int i = 0; i < flockmates.size(); ++i)
   {
      position += flockmates[i]->GetActorLocation();
   }
   position /= flockmates.size();
   FVector force = position - GetActorLocation();
   force /= force.Size();
   force *= myFlock->speed;
   force -= ProjectileMovementComponent->Velocity;
   force.Normalize(myFlock->max_force);

   if (force.Size() < myFlock->speed * 100 && force.Size() > -myFlock->speed * 100)
   {
      return force;
   }
   else
   {
      return FVector(0, 0, 0);
   }
}

// Steer toward closest target, if in perception range
FVector ABoid::Target()
{
   FVector force = FVector(0, 0, 0);
   ABait* nearbyBait = myFlock->baitManager->GetNearestBait(GetActorLocation());
   if (!nearbyBait)
   {
      return force;
   }
   force = nearbyBait->GetActorLocation() - GetActorLocation();
   force /= force.Size();
   force *= myFlock->speed;
   force -= ProjectileMovementComponent->Velocity;
   force.Normalize(myFlock->max_force);

   if (force.Size() < myFlock->speed * 100 && force.Size() > -myFlock->speed * 100)
   {
      return force;
   }
   else
   {
      return FVector(0, 0, 0);
   }
}

// Obstacle/bounds check
FVector ABoid::AvoidObstacles()
{
   FVector force = FVector(0, 0, 0);
   FVector loc = GetActorLocation();

   // Avoid beacon
   {
      float dist = FVector::Dist(loc, myFlock->beaconLocation);
      if (dist < myFlock->perceptionRange) {
         FVector diff = loc - myFlock->beaconLocation;
         diff *= diff.Size();
         diff /= dist;
         force += diff;
      }

      if (force.Size())
      {
         force /= force.Size();
         force *= myFlock->speed;
         force.Z -= ProjectileMovementComponent->Velocity.Z;
         force.Normalize(myFlock->max_force);
      }
   }

   if (force.Size() < myFlock->speed * 100 && force.Size() > -myFlock->speed * 100)
   {
      return force;
   }
   else
   {
      return FVector(0, 0, 0);
   }
}

// Steer away from player
FVector ABoid::AvoidPlayer()
{
   FVector force = FVector(0, 0, 0);
   FVector loc = GetActorLocation();

   float dist = FVector::Dist(loc, myFlock->player->GetActorLocation());
   if (dist < myFlock->perceptionRange) {
      FVector diff = loc - myFlock->player->GetActorLocation();
      diff *= diff.Size();
      diff /= dist;
      force += diff;
   }

   if (force.Size())
   {
      //if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Avoiding Player")));
      force /= force.Size();
      force *= myFlock->speed;
      force.Z -= ProjectileMovementComponent->Velocity.Z;
      force.Normalize(myFlock->max_force);
      // DrawDebugLine(GetWorld(), loc, loc + force * 40, FColor::Green, false, 1, 0, 1);
   }

   if (force.Size() < myFlock->speed * 100 && force.Size() > -myFlock->speed * 100)
   {
      return force;
   }
   else
   {
      return FVector(0, 0, 0);
   }
}

// Steer away from bounds
FVector ABoid::Bounds()
{
   FVector force = FVector(0, 0, 0);
   FVector loc = GetActorLocation();

   // Z bound check
   if (loc.Z < myFlock->perceptionRange / 2)
   {
      force.Z += myFlock->perceptionRange / 2 - (loc.Z);
   }
   else if (loc.Z > myFlock->bounds.Z - myFlock->perceptionRange / 3)
   {
      force.Z += (myFlock->bounds.Z - myFlock->perceptionRange / 3) - loc.Z;
   }

   // Check radial distance from center, circular bounds
   if (FVector2D::Distance((FVector2D)loc, (FVector2D)myFlock->bounds / 2) > myFlock->bounds.X - myFlock->perceptionRange / 3)
   {
      force += loc - myFlock->bounds / 2;
      // FVector2D newForce = (FVector2D)myFlock->bounds / 2 - (FVector2D)loc;
      // force.X += newForce.X;
      // force.Y += newForce.Y;
   }

   if (force.Size())
   {
      force /= force.Size();
      force *= myFlock->speed;
      force.Z -= ProjectileMovementComponent->Velocity.Z;
      force.Normalize(myFlock->max_force);
      // DrawDebugLine(GetWorld(), loc, loc + force * 40, FColor::Green, false, .1, 0, 1);
   }

   if (force.Size() < myFlock->speed * 100 && force.Size() > -myFlock->speed * 100)
   {
      return force;
   }
   else
   {
      return FVector(0, 0, 0);
   }
}

// move towards center of map
FVector ABoid::Centralize()
{
   FVector loc = GetActorLocation();
   FVector force = FVector(0, 0, 0);
   bool oob = false;
   // X bound check
   if ((loc.X < myFlock->perceptionRange * 2) || (loc.X > myFlock->bounds.X - myFlock->perceptionRange * 2))
   {
      oob = true;
   }

   // Y bound check
   if ((loc.Y < myFlock->perceptionRange * 2) || (loc.Y > myFlock->bounds.Y- myFlock->perceptionRange * 2))
   {
      oob = true;
   }

   force = myFlock->bounds / 2;
   force.Z = 0;
   force -= loc;

   force /= force.Size();
   force *= myFlock->speed;
   force -= ProjectileMovementComponent->Velocity;
   force.Normalize(myFlock->max_force);
   force += force * myFlock->centralize_weight;
   // DrawDebugLine(GetWorld(), loc, loc + force * 40, FColor::Green, false, .1, 0, 1);

   if (!oob)
   {
      force /= 5;
   }

   if (force.Size() < myFlock->speed * 100 && force.Size() > -myFlock->speed * 100)
   {
      return force;
   }
   else
   {
      return FVector(0, 0, 0);
   }
}

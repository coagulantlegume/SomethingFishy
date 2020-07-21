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

// Sets default values
ABoid::ABoid()
{
   // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
   PrimaryActorTick.bCanEverTick = true;

   // Set up visual mesh
   this->VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
   this->VisualMesh->SetupAttachment(RootComponent);

   static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/LPAnimals/Meshes/Fish.Fish"));

   if (SphereVisualAsset.Succeeded())
   {
      this->VisualMesh->SetStaticMesh(SphereVisualAsset.Object);
      this->VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
      this->VisualMesh->SetRelativeScale3D(FVector(0.4, 0.4, 0.4));
   }

   // Set up projectile component
   this->ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
   this->ProjectileMovementComponent->SetUpdatedComponent(VisualMesh);
   this->ProjectileMovementComponent->InitialSpeed = 0.0;
   this->ProjectileMovementComponent->MaxSpeed = this->speed;
   this->ProjectileMovementComponent->bRotationFollowsVelocity = true;
   this->ProjectileMovementComponent->bShouldBounce = true;
   this->ProjectileMovementComponent->Bounciness = 0.3f;
   this->ProjectileMovementComponent->ProjectileGravityScale = 0;

   VisualMesh->SetCollisionProfileName(TEXT("Boids"));
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
   Super::BeginPlay();
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
   Super::Tick(DeltaTime);

   std::vector<ABoid*> curr_flockmates;
   this->myFlock->AFlock::GetVisibleFlockmates(this, this->perceptionRange, curr_flockmates);

   // Make vector for current direction
   FVector moveDirection = GetActorRotation().Vector();

   // Calculate logic forces
   if (curr_flockmates.size())
   {
      moveDirection += this->Separation(curr_flockmates) * this->separation_weight;
      moveDirection += this->Alignment(curr_flockmates) * this->alignment_weight;
      moveDirection += this->Cohesion(curr_flockmates) * this->cohesion_weight;
   }
   if (this->myFlock->baitManager && this->myFlock->baitManager->NotEmpty())
   {
      moveDirection += this->Target() * this->target_weight;
   }
   moveDirection += this->Bounds() * this->bounds_weight;
   moveDirection += this->AvoidObstacles() * this->avoidObstacles_weight;
   moveDirection += this->AvoidPlayer() * this->avoidPlayer_weight;
   
   moveDirection += this->Centralize() * this->centralize_weight;

   // Temporary fix for occasional garbage moveDirection which propagates. fix later
   if (moveDirection.Size() < this->speed * 100 && moveDirection.Size() > -this->speed * 100)
   {
      this->ProjectileMovementComponent->Velocity += moveDirection * this->speed * DeltaTime;
   }
   else
   {
      this->ProjectileMovementComponent->Velocity = GetActorRotation().Vector() * this->speed * DeltaTime;
   }
}

// Separation: Steer to avoid crowding local flockmates
FVector ABoid::Separation(const std::vector<ABoid*>& flockmates)
{
   FVector force(0, 0, 0);
   FVector loc = this->GetActorLocation();
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
   force *= this->speed;
   force -= this->ProjectileMovementComponent->Velocity;
   force.Normalize(this->max_force);

   if (force.Size() < this->speed * 100 && force.Size() > -this->speed * 100)
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
   force *= this->speed;
   force -= this->ProjectileMovementComponent->Velocity;
   force.Normalize(this->max_force);

   if (force.Size() < this->speed * 100 && force.Size() > -this->speed * 100)
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
   FVector force = position - this->GetActorLocation();
   force /= force.Size();
   force *= this->speed;
   force -= this->ProjectileMovementComponent->Velocity;
   force.Normalize(this->max_force);

   if (force.Size() < this->speed * 100 && force.Size() > -this->speed * 100)
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
   ABait* nearbyBait = this->myFlock->baitManager->GetNearestBait(this->GetActorLocation());
   if (!nearbyBait)
   {
      return force;
   }
   force = nearbyBait->GetActorLocation() - this->GetActorLocation();
   force /= force.Size();
   force *= this->speed;
   force -= this->ProjectileMovementComponent->Velocity;
   force.Normalize(this->max_force);

   if (force.Size() < this->speed * 100 && force.Size() > -this->speed * 100)
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
   FVector loc = this->GetActorLocation();

   // Avoid beacon
   {
      float dist = FVector::Dist(loc, myFlock->beaconLocation);
      if (dist < perceptionRange) {
         FVector diff = loc - myFlock->beaconLocation;
         diff *= diff.Size();
         diff /= dist;
         force += diff;
      }

      if (force.Size())
      {
         force /= force.Size();
         force *= this->speed;
         force.Z -= this->ProjectileMovementComponent->Velocity.Z;
         force.Normalize(this->max_force);
      }
   }

   if (force.Size() < this->speed * 100 && force.Size() > -this->speed * 100)
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
   FVector loc = this->GetActorLocation();

   float dist = FVector::Dist(loc, myFlock->player->GetActorLocation());
   if (dist < perceptionRange / 4) {
      FVector diff = loc - myFlock->player->GetActorLocation();
      diff *= diff.Size();
      diff /= dist;
      force += diff;
   }

   if (force.Size())
   {
      force /= force.Size();
      force *= this->speed;
      force.Z -= this->ProjectileMovementComponent->Velocity.Z;
      force.Normalize(this->max_force);
   }
   
   // DrawDebugLine(GetWorld(), loc, loc + force * 4, FColor::Green, false, 1, 0, 1);

   if (force.Size() < this->speed * 100 && force.Size() > -this->speed * 100)
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
   FVector loc = this->GetActorLocation();

   // Z bound check
   if (loc.Z < this->perceptionRange / 2)
   {
      force.Z += this->perceptionRange / 2 - (loc.Z);
   }
   else if (loc.Z > this->myFlock->bounds.Z - this->perceptionRange / 3)
   {
      force.Z += (this->myFlock->bounds.Z - this->perceptionRange / 3) - loc.Z;
   }

   // Check radial distance from center, circular bounds
   if (FVector2D::Distance((FVector2D)loc, (FVector2D)this->myFlock->bounds / 2) > this->myFlock->bounds.X - this->perceptionRange / 3)
   {
      force += loc - this->myFlock->bounds / 2;
      // FVector2D newForce = (FVector2D)this->myFlock->bounds / 2 - (FVector2D)loc;
      // force.X += newForce.X;
      // force.Y += newForce.Y;
   }

   if (force.Size())
   {
      force /= force.Size();
      force *= this->speed;
      force.Z -= this->ProjectileMovementComponent->Velocity.Z;
      force.Normalize(this->max_force);
      // DrawDebugLine(GetWorld(), loc, loc + force * 40, FColor::Green, false, .1, 0, 1);
   }

   if (force.Size() < this->speed * 100 && force.Size() > -this->speed * 100)
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
   FVector loc = this->GetActorLocation();
   FVector force = FVector(0, 0, 0);
   bool oob = false;
   // X bound check
   if ((loc.X < this->perceptionRange * 2) || (loc.X > this->myFlock->bounds.X - this->perceptionRange * 2))
   {
      oob = true;
   }

   // Y bound check
   if ((loc.Y < this->perceptionRange * 2) || (loc.Y > this->myFlock->bounds.Y- this->perceptionRange * 2))
   {
      oob = true;
   }

   force = myFlock->bounds / 2;
   force.Z = 0;
   force -= loc;

   force /= force.Size();
   force *= this->speed;
   force -= this->ProjectileMovementComponent->Velocity;
   force.Normalize(this->max_force);
   force += force * centralize_weight;
   // DrawDebugLine(GetWorld(), loc, loc + force * 40, FColor::Green, false, .1, 0, 1);

   if (!oob)
   {
      force /= 5;
   }

   if (force.Size() < this->speed * 100 && force.Size() > -this->speed * 100)
   {
      return force;
   }
   else
   {
      return FVector(0, 0, 0);
   }
}

void ABoid::Remove()
{
   myFlock->Remove(this);
}

void ABoid::Enter()
{
   FVector loc(0, UKismetMathLibrary::RandomFloat() * this->myFlock->bounds.Y, 500);
   FRotator rot = (loc - this->myFlock->bounds / 2).Rotation();

   this->SetActorLocation(loc);
   this->SetActorRotation(rot);
}

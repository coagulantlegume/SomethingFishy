// Fill out your copyright notice in the Description page of Project Settings.

/*
   TODO: Change force functions to return desired direction, then modify application to use max force.
*/

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
   // PrimaryActorTick.TickInterval = 0.33f;

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
   ProjectileMovementComponent->Bounciness = 0.4f;
   ProjectileMovementComponent->ProjectileGravityScale = 0;

   VisualMesh->SetCollisionProfileName(TEXT("Boids"));
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
   Super::Tick(DeltaTime);

   std::vector<ABoid*> curr_flockmates;
   myFlock->AFlock::GetVisibleFlockmates(this, curr_flockmates);

   // Make vector for new force applied
   FVector new_force = GetActorRotation().Vector();
   float totalWeight = 0; // sum weights of all applied forces

   // Calculate flocking logic forces
   if (curr_flockmates.size())
   {
      new_force += Separation(curr_flockmates) * myFlock->separation_weight;
      new_force += Alignment(curr_flockmates) * myFlock->alignment_weight;
      new_force += Cohesion(curr_flockmates) * myFlock->cohesion_weight;

      totalWeight += myFlock->separation_weight + myFlock->alignment_weight + myFlock->cohesion_weight;
   }

   FVector temp = FVector::ZeroVector; // for temp desired vector storage to see if taking effect

   // Calculate targeting logic
   if (myFlock->target_weight && myFlock->baitManager && myFlock->baitManager->NotEmpty())
   {
      temp = Target() * myFlock->target_weight;
      if (temp.Size())
      {
         new_force += temp;
         totalWeight += myFlock->target_weight;
      }
   }

   // Calculate bounds logic
   if (myFlock->bounds_weight)
   {
      temp = Bounds() * myFlock->bounds_weight;
      if (temp.Size())
      {
         new_force += temp;
         totalWeight += myFlock->bounds_weight;
      }
   }

   // Calculate obstacle avoidance logic
   if (myFlock->avoidObstacles_weight)
   {
      temp = AvoidObstacles() * myFlock->avoidObstacles_weight;
      if (temp.Size())
      {
         new_force += temp;
         totalWeight += myFlock->avoidObstacles_weight;
      }
   }
   
   // Calculate player avoidance logic
   if (myFlock->avoidPlayer_weight)
   {
      temp = AvoidPlayer() * myFlock->avoidPlayer_weight;
      if (temp.Size())
      {
         new_force += temp;
         totalWeight += myFlock->avoidPlayer_weight;
      }
   }

   // Calculate centralizing logic
   if (myFlock->centralize_weight)
   {
      temp = Centralize() * myFlock->centralize_weight;
      if (temp.Size())
      {
         new_force += temp;
         totalWeight += myFlock->centralize_weight;
      }
   }

   // Calculate full desired velocity/direction with 0 < magnitude < speed
   if (totalWeight) // any logic influence to change direction
   {
      new_force /= totalWeight;
      //if (new_force.Size() > myFlock->max_speed)
      {
         new_force = new_force / new_force.Size() * myFlock->max_speed;
      }
   }
   else // no change, continue current heading up to speed
   {
      //if (new_force.Size() > myFlock->idle_speed)
      {
         new_force = new_force / new_force.Size() * myFlock->idle_speed;
      }
   }

   // Calculate new force
   new_force -= ProjectileMovementComponent->Velocity; // Optimal force
   if (new_force.Size() > (myFlock->max_force * DeltaTime)) // Cap force
   {
      new_force = new_force / new_force.Size() * (myFlock->max_force * DeltaTime);
   }

   
   // Apply movement
   ProjectileMovementComponent->Velocity += new_force;
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
      ProjectileMovementComponent->MaxSpeed = myFlock->max_speed;
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
   FVector force = FVector::ZeroVector;
   FVector loc = GetActorLocation();
   float shortestDist = myFlock->perceptionRange;
   for (unsigned int i = 0; i < flockmates.size(); ++i)
   {
      float dist = FVector::Dist(loc, flockmates[i]->GetActorLocation());
      if (dist < shortestDist)
      {
         shortestDist = dist;
      }
      FVector diff = loc - flockmates[i]->GetActorLocation();
      diff *= diff.Size();
      diff /= dist;
      force += diff;
   }

   force /= flockmates.size();
   float urgency = (myFlock->perceptionRange - shortestDist) / myFlock->perceptionRange;
   force = force / force.Size() * urgency;

   if (force.Size() < 1.001 && force.Size() > -1.001)
   {
      //if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d"), shortestDist));
      return force;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// Alignment: Steer towards the average heading of local flockmates
FVector ABoid::Alignment(const std::vector<ABoid*>& flockmates)
{
   FVector force = FVector::ZeroVector;
   FVector loc = GetActorLocation();
   float shortestDist = myFlock->perceptionRange;
   for (unsigned int i = 0; i < flockmates.size(); ++i)
   {
      float dist = FVector::Dist(loc, flockmates[i]->GetActorLocation());
      if (dist < shortestDist)
      {
         shortestDist = dist;
      }
      force += flockmates[i]->GetVelocity();
   }

   force /= flockmates.size();
   float urgency = (myFlock->perceptionRange - shortestDist) / myFlock->perceptionRange;
   force = force / force.Size() * urgency;

   if (force.Size() < 1.001 && force.Size() > -1.001)
   {
      return force;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// Cohesion: Steer to move toward the average position of local flockmates
FVector ABoid::Cohesion(const std::vector<ABoid*>& flockmates)
{
   FVector position = FVector::ZeroVector;
   for (unsigned int i = 0; i < flockmates.size(); ++i)
   {
      position += flockmates[i]->GetActorLocation();
   }
   position /= flockmates.size();
   FVector force = position - GetActorLocation();
   float dist = FVector::Dist(position, GetActorLocation());
   float urgency = (myFlock->perceptionRange - dist) / myFlock->perceptionRange;
   force = force / force.Size() * urgency;

   if (force.Size() < 1.001 && force.Size() > -1.001)
   {
      return force;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// Steer toward closest target, if in perception range
FVector ABoid::Target()
{
   FVector force = FVector::ZeroVector;
   ABait* nearbyBait = myFlock->baitManager->GetNearestBait(GetActorLocation());
   if (!nearbyBait)
   {
      return force;
   }

   float dist = FVector::Dist(nearbyBait->GetActorLocation(), GetActorLocation());
   if (dist < myFlock->perceptionRange * 5)
   {
      force = nearbyBait->GetActorLocation() - GetActorLocation();
      if (force.Size() > myFlock->perceptionRange * 5)
      {
         force = force / force.Size() * myFlock->perceptionRange * 5;
      }

      // Shrink to 0-1 scale
      force /= myFlock->perceptionRange * 5;
   }
   

   if (force.Size() < 1.001 && force.Size() > -1.001)
   {
      return force;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// Obstacle/bounds check
FVector ABoid::AvoidObstacles()
{
   FVector force = FVector::ZeroVector;
   FVector loc = GetActorLocation();

   // Avoid beacon
   float dist = FVector::Dist(loc, myFlock->beaconLocation);
   if (dist < 1200) {
      force = loc - myFlock->beaconLocation;
      force /= myFlock->max_speed;

      if (force.Size() > 1)
      {
         force /= force.Size();
         force *= myFlock->max_speed;
      }
   }

   if (force.Size() < 1.001 && force.Size() > -1.001)
   {
      return force;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// Steer away from player
FVector ABoid::AvoidPlayer()
{
   FVector force = FVector::ZeroVector;
   FVector loc = GetActorLocation();

   float dist = FVector::Dist(loc, myFlock->player->GetActorLocation());
   if (dist < myFlock->perceptionRange * 3) {
      // if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%s avoiding player"), this));
      force = loc - myFlock->player->GetActorLocation();
      force /= myFlock->max_speed;

      if (force.Size() > 1)
      {
         force /= force.Size();
         force *= myFlock->max_speed;
      }
   }

   if (force.Size() < 1.001 && force.Size() > -1.001)
   {
      return force;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// Steer away from bounds
FVector ABoid::Bounds()
{
   FVector force = FVector::ZeroVector;
   FVector loc = GetActorLocation();

   // Z bound check
   if (loc.Z < myFlock->perceptionRange / 2)
   {
      force.Z += myFlock->perceptionRange / 2 - loc.Z;
   }
   else if (loc.Z > myFlock->bounds.Z - myFlock->perceptionRange)
   {
      force.Z += myFlock->bounds.Z - myFlock->perceptionRange - loc.Z;
   }
   else // not cloes to floor or ceiling, level out to avoid constant bouncing
   {
      force.Z = -ProjectileMovementComponent->Velocity.Z / 10;
   }

   // Check radial distance from center, circular bounds
   if (FVector2D::Distance((FVector2D)loc, (FVector2D)myFlock->bounds / 2) > myFlock->bounds.X / 2 - myFlock->perceptionRange)
   {
      force += myFlock->bounds / 2 - loc;
      
      // Scale based on urgency
      force /= force.Size();
      force *= FVector2D::Distance((FVector2D)loc, (FVector2D)myFlock->bounds / 2) - (myFlock->bounds.X / 2 - myFlock->perceptionRange);
   }

   // Shrink and cap to 0-1 magnitude based on urgency
   force /= myFlock->max_speed;
   if (force.Size() > 1) // if desired movement exceeding max speed
   {
      force /= force.Size();
   }

   if (force.Size() < 1.001 && force.Size() > -1.001)
   {
      return force;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// move towards center of map
FVector ABoid::Centralize()
{
   FVector loc = GetActorLocation();
   FVector force = FVector::ZeroVector;

   force = myFlock->bounds / 2;
   force.Z = 0;
   force -= loc;

   if (force.Size() < myFlock->bounds.X / 4)
   {
      return FVector::ZeroVector;
   }

   // Scale based on urgency
   force /= force.Size();
   force *= FVector2D::Distance((FVector2D)loc, (FVector2D)myFlock->bounds / 2) - myFlock->bounds.X / 4;

   // Shrink and cap to 0-1 magnitude based on urgency
   force /= myFlock->max_speed;
   if (force.Size() > 1) // if desired movement exceeding max speed
   {
      force /= force.Size();
   }
   
   // DrawDebugLine(GetWorld(), loc, loc + force * 40, FColor::Green, false, .1, 0, 1);

   if (force.Size() < 1.001 && force.Size() > -1.001)
   {
      return force;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

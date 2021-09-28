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

   // Make vector for desired direction
   FVector desired_direction = GetActorRotation().Vector();
   float totalWeight = 0; // sum weights of all applied desired directions

   // Calculate flocking logic vector
   if (curr_flockmates.size())
   {
      desired_direction += Separation(curr_flockmates) * myFlock->separation_weight;
      desired_direction += Alignment(curr_flockmates) * myFlock->alignment_weight;
      desired_direction += Cohesion(curr_flockmates) * myFlock->cohesion_weight;

      totalWeight += myFlock->separation_weight + myFlock->alignment_weight + myFlock->cohesion_weight;
   }

   FVector temp = FVector::ZeroVector; // for temp desired vector storage to see if taking effect

   // Calculate targeting logic
   if (myFlock->target_weight && myFlock->baitManager && myFlock->baitManager->NotEmpty())
   {
      temp = Target() * myFlock->target_weight;
      if (temp.Size())
      {
         desired_direction += temp;
         totalWeight += myFlock->target_weight;
      }
   }

   // Calculate bounds logic
   if (myFlock->bounds_weight)
   {
      temp = Bounds() * myFlock->bounds_weight;
      if (temp.Size())
      {
         desired_direction += temp;
         totalWeight += myFlock->bounds_weight;
      }
   }

   // Calculate obstacle avoidance logic
   if (myFlock->avoidObstacles_weight)
   {
      temp = AvoidObstacles() * myFlock->avoidObstacles_weight;
      if (temp.Size())
      {
         desired_direction += temp;
         totalWeight += myFlock->avoidObstacles_weight;
      }
   }
   
   // Calculate player avoidance logic
   if (myFlock->avoidPlayer_weight)
   {
      temp = AvoidPlayer() * myFlock->avoidPlayer_weight;
      if (temp.Size())
      {
         desired_direction += temp;
         totalWeight += myFlock->avoidPlayer_weight;
      }
   }

   // Calculate centralizing logic
   if (myFlock->centralize_weight)
   {
      temp = Centralize() * myFlock->centralize_weight;
      if (temp.Size())
      {
         desired_direction += temp;
         totalWeight += myFlock->centralize_weight;
      }
   }

   // if (GEngine && ID == 0) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), desired_direction.Size()));

   // Calculate full desired velocity/direction with 0 < magnitude < maxSpeed
   if (totalWeight) // any logic influence to change direction
   {
      desired_direction /= totalWeight;
      desired_direction *= myFlock->max_speed;
      // if (GEngine && ID == 0) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), desired_direction.Size()));
      // if (desired_direction.Size() > myFlock->max_speed)
      // {
      //    desired_direction = desired_direction / desired_direction.Size() * myFlock->max_speed;
      // }
   }
   
   // Calculate new force
   FVector new_force = desired_direction - ProjectileMovementComponent->Velocity; // Optimal force
   if (new_force.Size() > (myFlock->max_force * DeltaTime)) // Cap force
   {
      if (GEngine && ID == 0) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), new_force.Size()));
      new_force = new_force / new_force.Size() * (myFlock->max_force * DeltaTime);
   }
   
   // Apply force
   ProjectileMovementComponent->Velocity += new_force;

   // Clamp velocity
   float vSize = abs(ProjectileMovementComponent->Velocity.Size());
   if (vSize > myFlock->max_speed)
   {
      ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity / vSize * myFlock->max_speed;
      // if (GEngine && ID == 0) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Max : %f %f"), vSize, myFlock->max_speed));
   }
   else if (vSize < myFlock->min_speed)
   {
      ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity / vSize * myFlock->min_speed;
      // if (GEngine && ID == 0) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Min : %f %f"), vSize, myFlock->min_speed));
   }
   // ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity.GetClampedToSize(myFlock->min_speed, myFlock->max_speed);
   
   // if (GEngine && ID == 0) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), ProjectileMovementComponent->Velocity.Size()));
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

// set identification number (for debug purposes)
void ABoid::SetID(unsigned int id)
{
   ID = id;
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
   FVector direction = FVector::ZeroVector;
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
      direction += diff;
   }

   direction /= flockmates.size();
   float urgency = (myFlock->perceptionRange - shortestDist) / myFlock->perceptionRange;
   direction = direction / direction.Size() * urgency;

   if (direction.Size() < 1.001 && direction.Size() > -1.001)
   {
      //if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d"), shortestDist));
      return direction;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// Alignment: Steer towards the average heading of local flockmates
FVector ABoid::Alignment(const std::vector<ABoid*>& flockmates)
{
   FVector direction = FVector::ZeroVector;
   FVector loc = GetActorLocation();
   float shortestDist = myFlock->perceptionRange;
   for (unsigned int i = 0; i < flockmates.size(); ++i)
   {
      float dist = FVector::Dist(loc, flockmates[i]->GetActorLocation());
      if (dist < shortestDist)
      {
         shortestDist = dist;
      }
      direction += flockmates[i]->GetVelocity();
   }

   direction /= flockmates.size();
   float urgency = (myFlock->perceptionRange - shortestDist) / myFlock->perceptionRange;
   direction = direction / direction.Size() * urgency;

   if (direction.Size() < 1.001 && direction.Size() > -1.001)
   {
      return direction;
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
   FVector direction = position - GetActorLocation();
   float dist = FVector::Dist(position, GetActorLocation());
   float urgency = (myFlock->perceptionRange - dist) / myFlock->perceptionRange;
   direction = direction / direction.Size() * urgency;

   if (direction.Size() < 1.001 && direction.Size() > -1.001)
   {
      return direction;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// Steer toward closest target, if in perception range
FVector ABoid::Target()
{
   FVector direction = FVector::ZeroVector;
   ABait* nearbyBait = myFlock->baitManager->GetNearestBait(GetActorLocation());
   if (!nearbyBait)
   {
      return direction;
   }

   float dist = FVector::Dist(nearbyBait->GetActorLocation(), GetActorLocation());
   if (dist < myFlock->perceptionRange * 5)
   {
      direction = nearbyBait->GetActorLocation() - GetActorLocation();
      if (direction.Size() > myFlock->perceptionRange * 5)
      {
         direction = direction / direction.Size() * myFlock->perceptionRange * 5;
      }

      // Shrink to 0-1 scale
      direction /= myFlock->perceptionRange * 5;
   }
   

   if (direction.Size() < 1.001 && direction.Size() > -1.001)
   {
      return direction;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// Obstacle/bounds check
FVector ABoid::AvoidObstacles()
{
   FVector direction = FVector::ZeroVector;
   FVector loc = GetActorLocation();

   // Avoid beacon
   float dist = FVector::Dist(loc, myFlock->beaconLocation);
   if (dist < 1200) {
      direction = loc - myFlock->beaconLocation;
      direction /= myFlock->max_speed;

      if (direction.Size() > 1)
      {
         direction /= direction.Size();
         direction *= myFlock->max_speed;
      }
   }

   if (direction.Size() < 1.001 && direction.Size() > -1.001)
   {
      return direction;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// Steer away from player
FVector ABoid::AvoidPlayer()
{
   FVector direction = FVector::ZeroVector;
   FVector loc = GetActorLocation();

   float dist = FVector::Dist(loc, myFlock->player->GetActorLocation());
   if (dist < myFlock->perceptionRange * 3) {
      // if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%s avoiding player"), this));
      direction = loc - myFlock->player->GetActorLocation();
      direction /= myFlock->max_speed;

      if (direction.Size() > 1)
      {
         direction /= direction.Size();
         direction *= myFlock->max_speed;
      }
   }

   if (direction.Size() < 1.001 && direction.Size() > -1.001)
   {
      return direction;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

// Steer away from bounds
FVector ABoid::Bounds()
{
   FVector direction = FVector::ZeroVector;
   FVector loc = GetActorLocation();

   // Z bound check
   if (loc.Z < myFlock->perceptionRange / 2)
   {
      direction.Z += myFlock->perceptionRange / 2 - loc.Z;
   }
   else if (loc.Z > myFlock->bounds.Z - myFlock->perceptionRange)
   {
      direction.Z += myFlock->bounds.Z - myFlock->perceptionRange - loc.Z;
   }
   else // not cloes to floor or ceiling, level out to avoid constant bouncing
   {
      direction.Z = -ProjectileMovementComponent->Velocity.Z / 10;
   }

   // Check radial distance from center, circular bounds
   if (FVector2D::Distance((FVector2D)loc, (FVector2D)myFlock->bounds / 2) > myFlock->bounds.X / 2 - myFlock->perceptionRange)
   {
      direction += myFlock->bounds / 2 - loc;
      
      // Scale based on urgency
      direction /= direction.Size();
      direction *= FVector2D::Distance((FVector2D)loc, (FVector2D)myFlock->bounds / 2) - (myFlock->bounds.X / 2 - myFlock->perceptionRange);
   }

   // Shrink and cap to 0-1 magnitude based on urgency
   direction /= myFlock->max_speed;
   if (direction.Size() > 1) // if desired movement exceeding max speed
   {
      direction /= direction.Size();
   }

   if (direction.Size() < 1.001 && direction.Size() > -1.001)
   {
      return direction;
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
   FVector direction = FVector::ZeroVector;

   direction = myFlock->bounds / 2;
   direction.Z = 0;
   direction -= loc;

   if (direction.Size() < myFlock->bounds.X / 4)
   {
      return FVector::ZeroVector;
   }

   // Scale based on urgency
   direction /= direction.Size();
   direction *= FVector2D::Distance((FVector2D)loc, (FVector2D)myFlock->bounds / 2) - myFlock->bounds.X / 4;

   // Shrink and cap to 0-1 magnitude based on urgency
   direction /= myFlock->max_speed;
   if (direction.Size() > 1) // if desired movement exceeding max speed
   {
      direction /= direction.Size();
   }
   
   // DrawDebugLine(GetWorld(), loc, loc + direction * 40, FColor::Green, false, .1, 0, 1);

   if (direction.Size() < 1.001 && direction.Size() > -1.001)
   {
      return direction;
   }
   else
   {
      return FVector::ZeroVector;
   }
}

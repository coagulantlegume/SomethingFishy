// Fill out your copyright notice in the Description page of Project Settings.

#include "Boid.h"
#include "Flock.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABoid::ABoid()
{
   // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
   PrimaryActorTick.bCanEverTick = true;

   // Set up visual mesh
   this->VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
   this->VisualMesh->SetupAttachment(RootComponent);

   static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

   if (SphereVisualAsset.Succeeded())
   {
      this->VisualMesh->SetStaticMesh(SphereVisualAsset.Object);
      this->VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
      this->VisualMesh->SetRelativeScale3D(FVector(0.25, 0.1, 0.1));
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
   if (this->InBounds() && curr_flockmates.size())
   {
      moveDirection += this->Separation(curr_flockmates) * this->separation_weight;
      moveDirection += this->Alignment(curr_flockmates) * this->alignment_weight;
      moveDirection += this->Cohesion(curr_flockmates) * this->cohesion_weight;
   }
   moveDirection += this->Target() * this->target_weight;
   moveDirection += this->AvoidObstacles() * this->avoidObstacles_weight;

   // Temporary fix for occasional garbage moveDirection which propagates. fix later
   if (moveDirection.Size() < this->speed * 100 && moveDirection.Size() > -this->speed * 100)
   {
      this->ProjectileMovementComponent->Velocity += moveDirection * this->speed * DeltaTime;
   }
   else
   {
      this->ProjectileMovementComponent->Velocity = FVector(1, 1, 1);
   }
}

// Separation: Steer to avoid crowding local flockmates
FVector ABoid::Separation(const std::vector<ABoid*>& flockmates)
{
   FVector force(0,0,0);
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

   return force;
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

   return force;
}

// Cohesion: Steer to move toward the average position of local flockmates
FVector ABoid::Cohesion(const std::vector<ABoid*>& flockmates)
{
   FVector position = FVector(0,0,0);
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

   return force;
}

// Steer toward closest target, if in perception range
FVector ABoid::Target()
{
   FVector force = FVector(0,0,0);
   return force;
}

// Obstacle/bounds check
FVector ABoid::AvoidObstacles()
{
   FVector force = FVector(0,0,0);
   FVector loc = this->GetActorLocation();
   // Avoid ground
   if (loc.Z < this->perceptionRange / 3)
   {
      force.Z += this->perceptionRange / 3 - loc.Z;
   }
   // Avoid going too high
   else if (loc.Z > this->myFlock->bounds.Z - this->perceptionRange / 3)
   {
      force.Z += (this->myFlock->bounds.Z - this->perceptionRange / 3) - loc.Z;
      //UE_LOG(LogTemp, Warning, TEXT("%f"), this->GetActorLocation().Z)
   }

   if (force.Size()) 
   {
      force /= force.Size();
      force *= this->speed;
      force.Z -= this->ProjectileMovementComponent->Velocity.Z;
      force.Normalize(this->max_force);
   }

   return force;
}

// Check bounds, start fade if out of bounds
bool ABoid::InBounds()
{
   // AABB bounds checking
   FVector loc = this->GetActorLocation();
   float diffX = this->myFlock->bounds.X / 2 - abs(loc.X);
   float diffY = this->myFlock->bounds.Y / 2 - abs(loc.Y);
   bool oob = true; // out of bounds flag

   if (diffX < -this->myFlock->boundsBuffer)
   {
      this->SetActorLocation(FVector(
         -loc.X, 
         UKismetMathLibrary::RandomFloat() * this->myFlock->bounds.Y - this->myFlock->bounds.Y / 2, 
         loc.Z
      ));
      oob = false;
   }
   if (diffY < -this->myFlock->boundsBuffer)
   {
      this->SetActorLocation(FVector(
         UKismetMathLibrary::RandomFloat() * this->myFlock->bounds.X - this->myFlock->bounds.X / 2, 
         -loc.Y, 
         loc.Z
      ));
      oob = false;
   }
   return oob;
}

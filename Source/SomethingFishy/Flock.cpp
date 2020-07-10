// Fill out your copyright notice in the Description page of Project Settings.

#include "Flock.h"
#include "Boid.h"
#include "BaitManager.h"

#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFlock::AFlock()
{
   // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
   PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AFlock::BeginPlay()
{
   Super::BeginPlay();

   UWorld* world = GetWorld();
   FVector location(0);
   FRotator rotation(0);

   // Generate all boids in flock randomly on playable area
   for (int i = 0; i < this->flockSize; ++i)
   {
      // set random position within bounds
      location.FVector::Set(UKismetMathLibrary::RandomFloat() * this->bounds.X - this->bounds.X / 2,
                            UKismetMathLibrary::RandomFloat() * this->bounds.Y - this->bounds.Y / 2,
                            UKismetMathLibrary::RandomFloat() * this->bounds.Z);
      ABoid* newFlockmate = (ABoid*)world->UWorld::SpawnActor(ActorToSpawn, &location, 0);
      newFlockmate->myFlock = this;
      this->flockmates.push_back(newFlockmate);
   }

   // TODO: Have world bait manager variable to connect to so player has access
   this->baitManager = (ABaitManager*)world->UWorld::SpawnActor(BaitManagerActor, 0, 0);
   this->baitManager->ActorToSpawn = this->BaitActorToSpawn;
   this->baitManager->SpawnBait(FVector(0, 0, 0)); // spawn one bait, just to test
}

// Return visible flock mates
void AFlock::GetVisibleFlockmates(ABoid* me, float perceptionRange, std::vector<ABoid*>& visibleBoids)
{
   for (auto const& i : flockmates) {
      if (FVector::Dist(i->GetActorLocation(), me->GetActorLocation()) < perceptionRange &&
          me != i) {
         visibleBoids.push_back(i);
      }
   }
}

// Remove a specific boid from world
void AFlock::Remove(ABoid* toRemove)
{
   for (std::list<ABoid*>::iterator it = flockmates.begin(); it != flockmates.end(); ++it) {
      if (*it == toRemove) {
         flockmates.erase(it);
         (*it)->ConditionalBeginDestroy();
         return;
      }
   }
}

// Respawn boid at edge of world
//void AFlock::Respawn(ABoid* toRespawn)
//{
//   toRespawn->SetActorLocation()
//}
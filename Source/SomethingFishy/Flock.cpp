// Fill out your copyright notice in the Description page of Project Settings.

#include "Flock.h"
#include "Boid.h"
#include "BaitManager.h"
#include "PlayerPawn.h"
#include "ShopKeep.h"

#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

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
      location.FVector::Set(UKismetMathLibrary::RandomFloat() * this->bounds.X,
         UKismetMathLibrary::RandomFloat() * this->bounds.Y,
         2000);
      rotation = FRotator(UKismetMathLibrary::RandomFloat() * 180,
         UKismetMathLibrary::RandomFloat() * 180, 0);
      ABoid* newFlockmate = (ABoid*)world->UWorld::SpawnActor(ActorToSpawn, &location, &rotation);
      newFlockmate->myFlock = this;
      this->flockmates.push_back(newFlockmate);
   }

   // Calculate beacon location
   FVector shopLoc = shopKeep->GetActorLocation();
   beaconLocation = FVector(shopLoc.X, shopLoc.Y, shopLoc.Z + 500);
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

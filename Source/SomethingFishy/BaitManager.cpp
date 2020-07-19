// Fill out your copyright notice in the Description page of Project Settings.


#include "BaitManager.h"
#include "Bait.h"

// Sets default values
ABaitManager::ABaitManager()
{
   // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
   PrimaryActorTick.bCanEverTick = true;

   // reserve minimal expected space for bait
   worldBait.reserve(4);
}

// Called when the game starts or when spawned
void ABaitManager::BeginPlay()
{
   Super::BeginPlay();
}

// Called every frame
void ABaitManager::Tick(float DeltaTime)
{
   Super::Tick(DeltaTime);
   for (int i = 0; i < this->worldBait.size(); ++i) {
      if (this->worldBait[i]->value <= 0)
      {
         // if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("HERE")));
         this->worldBait.erase(this->worldBait.begin() + i);
      }
   }
}

// Check if there is any bait currently spawned in the world
bool ABaitManager::NotEmpty()
{
   return !this->worldBait.empty();
}

// Spawn bait at specified location
void ABaitManager::SpawnBait(const FVector& loc)
{
   UWorld* world = GetWorld();
   ABait* newBait = (ABait*)world->UWorld::SpawnActor(ActorToSpawn, &loc, 0);
   newBait->baitManager = this;
   this->worldBait.push_back(newBait);
}

// Get bait nearest location
ABait* ABaitManager::GetNearestBait(const FVector& loc)
{
   if (!this->worldBait.size())
   {
      return NULL;
   }

   ABait* nearestBait = this->worldBait[0];
   float nearestDist = 9999;
   for (int i = 0; i < this->worldBait.size(); ++i) {
      float dist = FVector::Dist(loc, this->worldBait[i]->GetActorLocation());
      if (dist < nearestDist)
      {
         nearestBait = this->worldBait[i];
         nearestDist = dist;
      }
   }
   if (nearestDist < 1500) {
      return nearestBait;
   }
   return NULL;
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "Flock.h"
#include "Boid.h"
#include "BaitManager.h"
#include "PlayerPawn.h"
#include "ShopKeep.h"
#include "CellGrid.h"

#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

/****************************************
   *  Basic Public Functions
*****************************************/
// Sets default values
AFlock::AFlock()
{
   // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
   PrimaryActorTick.bCanEverTick = false;

   // create cell grid component
   flockGrid = CreateDefaultSubobject<UCellGrid>(TEXT("CellGrid"));
   flockGrid->MakeDynamic();
}

// Return number of boids in flock
int32 AFlock::GetFlockSize()
{
   return this->flockSize;
}

// Return visible flock mates
void AFlock::GetVisibleFlockmates(ABoid* me, std::vector<ABoid*>& visibleBoids)
{
   std::vector<AActor*> neighbors = flockGrid->GetNeighbors(me);
   for (auto const& i : neighbors) {
      if (FVector::Dist(i->GetActorLocation(), me->GetActorLocation()) < perceptionRange &&
         me != i) {
         visibleBoids.push_back(dynamic_cast<ABoid*>(i));
      }
   }
}

// Remove a specific boid from world
void AFlock::Remove(ABoid* toRemove)
{
   flockGrid->RemoveElement(toRemove);
}

// Called when the game starts or when spawned
void AFlock::BeginPlay()
{
   Super::BeginPlay();

   UWorld* world = GetWorld();
   FVector location(0);
   FRotator rotation(0);

   flockGrid->setUnitSize(perceptionRange);

   // Generate all boids in flock randomly on playable area
   for (int i = 0; i < this->flockSize; ++i)
   {
      // set random position within bounds
      location.FVector::Set(UKismetMathLibrary::RandomFloat() * this->bounds.X,
         UKismetMathLibrary::RandomFloat() * this->bounds.Y,
         500);
      rotation = FRotator(UKismetMathLibrary::RandomFloat() * 180,
         UKismetMathLibrary::RandomFloat() * 180, 0);
      ABoid* newFlockmate = (ABoid*)world->UWorld::SpawnActor(ActorToSpawn, &location, &rotation);
      newFlockmate->SetFlock(this);
      newFlockmate->SetID(i);
      flockGrid->AddElement(newFlockmate);
   }

   // Calculate beacon location
   if (shopKeep)
   {
      FVector shopLoc = shopKeep->GetActorLocation();
      beaconLocation = FVector(shopLoc.X, shopLoc.Y, shopLoc.Z + 500);
   }
}
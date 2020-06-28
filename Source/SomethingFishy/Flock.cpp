// Fill out your copyright notice in the Description page of Project Settings.


#include "Flock.h"
#include "Boid.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFlock::AFlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set navigation bounds
	this->bounds = FVector(200, 200, 200);
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
		location.FVector::Set(UKismetMathLibrary::RandomFloat() * this->bounds.X,
									 UKismetMathLibrary::RandomFloat() * this->bounds.Y,
									 UKismetMathLibrary::RandomFloat() * this->bounds.Z);
		this->flockmates.push_back((ABoid*)world->UWorld::SpawnActor(ActorToSpawn, &location, 0));
	}
}

// Return visible flock mates
void AFlock::GetVisibleFlockmates(ABoid* me, float perceptionRange, std::vector<ABoid*>& visibleBoids)
{
	for (int32 i = 0; i < this->flockSize; ++i) {
		if (FVector::Dist(this->flockmates[i]->GetActorLocation(), me->GetActorLocation()) < perceptionRange) {
			visibleBoids.push_back(this->flockmates[i]);
		}
	}
}
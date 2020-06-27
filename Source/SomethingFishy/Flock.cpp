// Fill out your copyright notice in the Description page of Project Settings.


#include "Flock.h"
#include "Boid.h"
#include "Engine/World.h"

// Sets default values
AFlock::AFlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set navigation bounds
	this->bounds = FVector(200, 200, 200);

	UWorld* world = GetWorld();
	//FVector* location = FVector(0, 0, 0);
	//FRotator* rotation = FRotator(0, 0, 0);
	//world->SpawnActor(ActorToSpawn, location, rotation);

	// Generate all boids in flock randomly on playable area
	//for (int i = 0; i < this->flockSize; ++i)
	//{
	//	this->flockmates.push_back(
	//		GetWorld()->SpawnActor<ABoid>(ToSpawn, FVector(FMath::RandRange(0,200), 0, FMath::RandRange(0,200)),
	//		this->GetActorRotation())
	//	);
	//}
}

// Called when the game starts or when spawned
void AFlock::BeginPlay()
{
	Super::BeginPlay();
	
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
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flock.generated.h"

class ABoid;

UCLASS()
class SOMETHINGFISHY_API AFlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Return number of boids in flock
	int32 GetFlockSize() { return this->flockSize; }

	// Return visible flock mates
	void GetVisibleFlockmates(ABoid* me, float perceptionRange, std::vector<ABoid*>& visibleBoids);

	// Array of all boids in flock
	std::vector<ABoid*> flockmates;

	// Number of boids in flock
	UPROPERTY(EditAnywhere)
		int32 flockSize = 20;

	// Bounding limits of flock movement
	UPROPERTY(EditAnywhere)
		FVector bounds;

	// UClass of actors to spawn as flockmates
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> ActorToSpawn;
};

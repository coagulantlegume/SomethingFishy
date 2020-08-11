// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaitManager.generated.h"

UCLASS()
class SOMETHINGFISHY_API ABaitManager : public AActor
{
	GENERATED_BODY()

public:
	/****************************************
	*  Basic Public Functions
	*****************************************/
	/* Constructor */
	ABaitManager();

	/* Tick */
	virtual void Tick(float DeltaTime) override;

	/* Getters */
	// Get bait nearest location
	class ABait* GetNearestBait(const FVector& loc);

	// Check if there is any bait currently spawned in the world
	bool NotEmpty();

	/* Manipulators */
	// Spawn bait at specified location
	void SpawnBait(const FVector& loc);


	/****************************************
	*  Particle reference pointers
	*****************************************/
	// Scent particle system
	UPROPERTY(EditAnywhere)
		UParticleSystem* ScentParticle;

	// Bite particle system
	UPROPERTY(EditAnywhere)
		UParticleSystem* BiteParticle;


	/****************************************
	*  Spawn class types
	*****************************************/
	// UClass of actors to spawn as bait
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABait> ActorToSpawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private: 
	// Container of bait 
	std::vector<class ABait*> worldBait;
};

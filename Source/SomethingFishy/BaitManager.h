// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaitManager.generated.h"

class ABait;

UCLASS()
class SOMETHINGFISHY_API ABaitManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaitManager();

	// Get constant vector container of current bait elements in the world
	ABait* GetBait() const;

	// Check if there is any bait currently spawned in the world
	bool NotEmpty();

	// Spawn bait at specified location
	void SpawnBait(const FVector& loc);

	// Get bait nearest location
	ABait* GetNearestBait(const FVector& loc);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Container of bait 
	std::vector<ABait*> worldBait;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// UClass of actors to spawn as bait
	UPROPERTY(EditAnywhere)
		TSubclassOf<ABait> ActorToSpawn;

	// Scent particle system
	 UPROPERTY(EditAnywhere)
	 	UParticleSystem* ScentParticle;

	 // Bite particle system
	 UPROPERTY(EditAnywhere)
		 UParticleSystem* BiteParticle;
};

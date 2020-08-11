// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bait.generated.h"

UCLASS()
class SOMETHINGFISHY_API ABait : public AActor
{
	GENERATED_BODY()

public:
	/****************************************
	*  Basic Public Functions
	*****************************************/
	/* Constructor */
	ABait();

	/* Tick */
	virtual void Tick(float DeltaTime) override;

	/* Getters */
	int GetValue();

	/* Setters */
	void SetManager(class ABaitManager* manager);

	/* Collision event handler */
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/****************************************
	*  Member Variables
	*****************************************/
	/* Component References */
	UStaticMeshComponent* VisualMesh;

	/* Particle References*/
	UParticleSystemComponent* scentParticle;

	UParticleSystemComponent* biteParticle;

	/* Parameters */
	// Current value (bites left)
	float value = 16;

	// spawn location
	FVector spawnLocation;

	// reference to world bait manager
	class ABaitManager* baitManager;
};

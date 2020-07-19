// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bait.generated.h"

class ABaitManager;
class UBoxComponent;

UCLASS()
class SOMETHINGFISHY_API ABait : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABait();

	// mesh body
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* VisualMesh;

	UPROPERTY(VisibleAnywhere)
		UParticleSystemComponent* scentParticle;

	UPROPERTY(VisibleAnywhere)
		UParticleSystemComponent* biteParticle;

	// Current value (bites left)
	UPROPERTY(VisibleAnywhere)
		float value = 16;

	// spawn location
	UPROPERTY(VisibleAnywhere)
		FVector spawnLocation;

	ABaitManager* baitManager;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Bite collision
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);
};

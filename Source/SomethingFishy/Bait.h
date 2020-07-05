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
	// Sets default values for this actor's properties
	ABait();

	// mesh body to be replaced later
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* VisualMesh;

	// Current value (bites left)
	UPROPERTY(VisibleAnywhere)
		float value = 16;

	// spawn location
	UPROPERTY(VisibleAnywhere)
		FVector spawnLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Bite collision
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);
};
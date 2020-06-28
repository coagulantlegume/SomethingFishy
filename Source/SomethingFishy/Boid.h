// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

class AFlock;

UCLASS()
class SOMETHINGFISHY_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoid();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* VisualMesh;

	UPROPERTY(EditAnywhere)
	AFlock* myFlock;

	UPROPERTY(EditAnywhere)
	float perceptionRange = 300;

	UPROPERTY(EditAnywhere)
		float speed = 100;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Separation: Steer to avoid crowding local flockmates
	void Separation(const std::vector<ABoid*>& flockMates, FVector moveDirection);

	// Alignment: Steer towards the average heading of local flockmates
	void Alignment(const std::vector<ABoid*>& flockMates, FVector moveDirection);

	// Cohesion: Steer to move toward the average position of local flockmates
	void Cohesion(const std::vector<ABoid*>& flockMates, FVector moveDirection);
};

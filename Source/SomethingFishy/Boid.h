// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

class AFlock;
class UProjectileMovementComponent;

UCLASS()
class SOMETHINGFISHY_API ABoid : public AActor
{
   GENERATED_BODY()
   
public:	
   // Sets default values for this actor's properties
   ABoid();

   // mesh body to be replaced later
   UPROPERTY(VisibleAnywhere)
   UStaticMeshComponent* VisualMesh;

   // movement component for velocity
   UPROPERTY(VisibleAnywhere)
   UProjectileMovementComponent* ProjectileMovementComponent;

   UPROPERTY(EditAnywhere)
   AFlock* myFlock;

   UPROPERTY(EditAnywhere)
   float perceptionRange = 300;

   UPROPERTY(EditAnywhere)
   float speed = 400;

   UPROPERTY(EditAnywhere)
   float separation_weight = 1.5;

   UPROPERTY(EditAnywhere)
   float alignment_weight = 2;

   UPROPERTY(EditAnywhere)
   float cohesion_weight = 2.5;

   UPROPERTY(EditAnywhere)
   float target_weight = 0;

   UPROPERTY(EditAnywhere)
   float avoidObstacles_weight = 1.5;

   UPROPERTY(EditAnywhere)
      float max_force = 1;

protected:
   // Called when the game starts or when spawned
   virtual void BeginPlay() override;

public:	
   // Called every frame
   virtual void Tick(float DeltaTime) override;

   // Separation: Steer to avoid crowding local flockmates
   FVector Separation(const std::vector<ABoid*>& flockMates);

   // Alignment: Steer towards the average heading of local flockmates
   FVector Alignment(const std::vector<ABoid*>& flockMates);

   // Cohesion: Steer to move toward the average position of local flockmates
   FVector Cohesion(const std::vector<ABoid*>& flockMates);

   // Steer toward closest target, if in perception range
   FVector Target();

   // Steer away from obstacles
   FVector AvoidObstacles();

   // Check bounds, start fade if out of bounds
   bool InBounds();
};

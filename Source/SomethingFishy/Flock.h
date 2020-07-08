// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flock.generated.h"

class ABoid;
class ABaitManager;
class ABait;
class UMaterialInstance;

UCLASS()
class SOMETHINGFISHY_API AFlock : public AActor
{
   GENERATED_BODY()
   
public:	
   // Sets default values for this actor's properties
   AFlock();

   UPROPERTY(VisibleAnywhere)
      ABaitManager* baitManager;

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
      FVector bounds = FVector(200,200,200);

   // Buffer for turning off senses outside of bounds before teleporting
   UPROPERTY(EditAnywhere)
      float boundsBuffer = 100;

   // UClass of actors to spawn as flockmates
   UPROPERTY(EditAnywhere)
      TSubclassOf<ABoid> ActorToSpawn;

   // UClass of baitmanager actor
   UPROPERTY(EditAnywhere)
      TSubclassOf<ABaitManager> BaitManagerActor;

   // UClass of actors to spawn as bait
   UPROPERTY(EditAnywhere)
      TSubclassOf<ABait> BaitActorToSpawn;
};

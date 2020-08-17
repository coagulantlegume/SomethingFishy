// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flock.generated.h"

UCLASS()
class SOMETHINGFISHY_API AFlock : public AActor
{
   GENERATED_BODY()

public:
   /****************************************
   *  Basic Public Functions
   *****************************************/
   /* Constructors */
   AFlock();

   /* Getters */
   // Return number of boids in flock
   int32 GetFlockSize();

   // Return visible flock mates
   void GetVisibleFlockmates(class ABoid* me, std::vector<class ABoid*>& visibleBoids);

   /* Manipulators */
   // Remove a specific boid from world
   void Remove(class ABoid* toRemove);


   /****************************************
   *  Actor reference pointers
   *****************************************/
   UPROPERTY(EditAnywhere)
      class ABaitManager* baitManager;

   UPROPERTY(EditAnywhere)
      class APlayerPawn* player;

   UPROPERTY(EditAnywhere)
      class AShopKeep* shopKeep;

   FVector beaconLocation;


   /****************************************
   *  Spawn class types
   *****************************************/
   // UClass of actors to spawn as flockmates
   UPROPERTY(EditAnywhere)
      TSubclassOf<class ABoid> ActorToSpawn;


   /****************************************
   *  Flock Parameters
   *****************************************/
   // Number of boids in flock
   UPROPERTY(EditAnywhere)
      int32 flockSize = 20;

   // Bounding limits of flock movement
   UPROPERTY(EditAnywhere)
      FVector bounds = FVector(200, 200, 200);


   /****************************************
   *  Boid Parameters
   *****************************************/
   UPROPERTY(EditAnywhere)
      float perceptionRange = 300;

   UPROPERTY(EditAnywhere)
      float speed = 500;

   UPROPERTY(EditAnywhere)
      float max_force = .5;

   /* Behavior weights */
   UPROPERTY(EditAnywhere)
      float separation_weight = 2;

   UPROPERTY(EditAnywhere)
      float alignment_weight = 1.5;

   UPROPERTY(EditAnywhere)
      float cohesion_weight = 1;

   UPROPERTY(EditAnywhere)
      float target_weight = 2;

   UPROPERTY(EditAnywhere)
      float avoidObstacles_weight = 2;

   UPROPERTY(EditAnywhere)
      float bounds_weight = 1;

   UPROPERTY(EditAnywhere)
      float avoidPlayer_weight = 6;

   UPROPERTY(EditAnywhere)
      float centralize_weight = 1;

protected:
   // Called when the game starts or when spawned
   virtual void BeginPlay() override;

private:
   // Array of all boids in flock
   std::list <class ABoid* > flockmates;
   class UCellGrid* flockGrid;
};
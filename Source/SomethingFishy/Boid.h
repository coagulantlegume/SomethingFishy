// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

UCLASS()
class SOMETHINGFISHY_API ABoid : public AActor
{
   GENERATED_BODY()

public:
   /****************************************
   *  Basic Public Functions
   *****************************************/
   /* Constructor */
   ABoid();

   /* Tick */
   virtual void Tick(float DeltaTime) override;

   /* Manipulators */
   // Set the flock that boid is a member of
   void SetFlock(class AFlock* flock);

   // remove from world
   void Remove();

   // set rotation and position to enter/re-enter main play area
   void Enter();

   // set identification number (for debug purposes)
   void SetID(unsigned int id);

protected:
   // Called when the game starts or when spawned
   virtual void BeginPlay() override;

private:
   /****************************************
   *  Member Variables
   *****************************************/
   /* Component References */
   UStaticMeshComponent* VisualMesh;

   class UProjectileMovementComponent* ProjectileMovementComponent;

   /* Parameters */
   class AFlock* myFlock; // reference to flock that boid is a member of

   /* Identification */
   unsigned int ID;


   /****************************************
   *  Behavior Functions
   *****************************************/
   // Separation: Steer to avoid crowding local flockmates
   FVector Separation(const std::vector<class ABoid*>& flockMates);

   // Alignment: Steer towards the average heading of local flockmates
   FVector Alignment(const std::vector<class ABoid*>& flockMates);

   // Cohesion: Steer to move toward the average position of local flockmates
   FVector Cohesion(const std::vector<class ABoid*>& flockMates);

   // Steer toward closest target, if in perception range
   FVector Target();

   // Steer away from obstacles
   FVector AvoidObstacles();

   // Steer away from player
   FVector AvoidPlayer();

   // Steer away from bounds
   FVector Bounds();

   // move towards center of map
   FVector Centralize();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCapsuleComponent;
class UMyPawnMovementComponent;
class AArms;
class ABaitManager;

UCLASS()
class SOMETHINGFISHY_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

   UMyPawnMovementComponent* movementComponent;
   virtual UPawnMovementComponent* GetMovementComponent() const override;


   UPROPERTY(VisibleAnywhere)
   UCapsuleComponent* collisionMesh;

   UPROPERTY(EditAnywhere)
   USpringArmComponent* springArm;

   UPROPERTY(EditAnywhere)
   UCameraComponent* camera;

   UPROPERTY(EditAnywhere)
      ABaitManager* baitManager;

   float maxSpeed = 7;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
   // Called every frame
   virtual void Tick(float DeltaSeconds) override;

   // Called to bind functionality to input
   virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

   // Input functions
   void Move_XAxis(float value);
   void Move_YAxis(float value);
   void CameraMoveX(float value);
   void CameraMoveY(float value);
   void Interact();
   void PlaceBait();
   FHitResult TraceCollision(float dist);

   // Input variables
   FVector CurrentVelocity;
   FVector mouseInput;
   //FVector moveInput;
   bool placingBait;

   // The distance player can reach objects
   float reachDistance = 500;
};

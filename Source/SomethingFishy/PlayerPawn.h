// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class SOMETHINGFISHY_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

   class UMyPawnMovementComponent* movementComponent;
   virtual class UPawnMovementComponent* GetMovementComponent() const override;


   UPROPERTY(VisibleAnywhere)
   class UCapsuleComponent* collisionMesh;

   UPROPERTY(EditAnywhere)
   class USpringArmComponent* springArm;

   UPROPERTY(EditAnywhere)
   class UCameraComponent* camera;

   UPROPERTY(EditAnywhere)
   class ABaitManager* baitManager;

   class APrimary_HUD* primaryHUD;

   float maxSpeed = 7;

   float jumpImpulse = 200000;
   float jumpHeight = 150;

   int numFish = 0;
   UPROPERTY(EditAnywhere)
   int numBait = 0;

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
   void Jump();
   void Interact();
   void PlaceBait();
   FHitResult TraceCollision(float dist);

   // Hit event
   void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);

   // Input variables
   FVector CurrentVelocity;
   FVector mouseInput;
   //FVector moveInput;
   bool placingBait;

   // The distance player can reach objects
   float reachDistance = 500;

private:
   bool bjumping = false;
   bool bfalling = false;
   float jumpStartZ = 0;
};

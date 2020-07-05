// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class SOMETHINGFISHY_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

   UPROPERTY(VisibleAnywhere)
   UStaticMeshComponent* VisualMesh;

   float maxSpeed = 800;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

   UPROPERTY(EditAnywhere)
   USpringArmComponent* springArm;
   UCameraComponent* camera;

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
   void PlaceBait();

   // Input variables
   FVector CurrentVelocity;
   FVector mouseInput;
   FVector moveInput;
   bool placingBait;
};

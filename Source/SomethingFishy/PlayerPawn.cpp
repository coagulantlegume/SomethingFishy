// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

   // Set this pawn to be controlled by the lowest-numbered player
   AutoPossessPlayer = EAutoReceiveInput::Player0;

   // Create collision object and set as root
   collisionMesh = CreateDefaultSubobject<UCapsuleComponent>(TEXT("collisionMesh"));
   RootComponent = collisionMesh;
   collisionMesh->InitCapsuleSize(60, 160);
   collisionMesh->SetCollisionProfileName(TEXT("Player"));
   collisionMesh->SetupAttachment(RootComponent);

   springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
   springArm->SetupAttachment(RootComponent);
   springArm->SetRelativeLocationAndRotation(FVector(-60.0f, 0.0f, 140.f), FRotator(0.0f, 0.0f, 0.0f));
   springArm->TargetArmLength = -60;
   springArm->bEnableCameraLag = false;
   camera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
   camera->SetupAttachment(springArm, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

   // Handle movement based on our "MoveX" and "MoveY" axes
   if (!moveInput.IsZero())
   {
      moveInput = moveInput.GetClampedToMaxSize(maxSpeed);
      //FVector NewLocation = GetActorLocation() + (moveInput * DeltaTime);
      AddActorLocalOffset(moveInput, true);
      //collisionMesh->BodyInstance.SetLinearVelocity(NewLocation, false, true);
      //if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Velocity: (%d, %d, %d)"), collisionMesh->BodyInstance.));
      moveInput *= 0.2;
   }
   

   FRotator  newYaw = GetActorRotation();
   newYaw.Yaw += mouseInput.X;
   SetActorRotation(newYaw);

   FRotator newPitch = camera->GetComponentRotation();
   newPitch.Pitch = FMath::Clamp(newPitch.Pitch + mouseInput.Y, -80.f, 80.f);
   camera->SetWorldRotation(newPitch);
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

   // Respond when PlaceBait key is pressed or released.
   InputComponent->BindAction("PlaceBait", IE_Pressed, this, &APlayerPawn::PlaceBait);
   
   // Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
   InputComponent->BindAxis("XMove", this, &APlayerPawn::Move_XAxis);
   InputComponent->BindAxis("YMove", this, &APlayerPawn::Move_YAxis);

   // Set up "look" bindings.
   PlayerInputComponent->BindAxis("Turn", this, &APlayerPawn::CameraMoveX);
   PlayerInputComponent->BindAxis("LookUp", this, &APlayerPawn::CameraMoveY);
}

void APlayerPawn::Move_XAxis(float value)
{

   moveInput.X += FMath::Clamp(value, -1.0f, 1.0f) * maxSpeed;
}

void APlayerPawn::Move_YAxis(float value)
{
   moveInput.Y += FMath::Clamp(value, -1.0f, 1.0f) * maxSpeed;
}

void APlayerPawn::CameraMoveX(float value)
{
   mouseInput.X = value;
}

void APlayerPawn::CameraMoveY(float value)
{
   mouseInput.Y = -value;
}

void APlayerPawn::PlaceBait()
{
   placingBait = true;
}

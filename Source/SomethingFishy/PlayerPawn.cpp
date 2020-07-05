// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

   // Set this pawn to be controlled by the lowest-numbered player
   AutoPossessPlayer = EAutoReceiveInput::Player0;
   
   // Create a dummy root component we can attach things to.
   RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
   // Create a visible object
   VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));

   //static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
   //
   //if (CubeVisualAsset.Succeeded())
   //{
   //   this->VisualMesh->SetStaticMesh(CubeVisualAsset.Object);
   //   this->VisualMesh->SetRelativeScale3D(FVector(.01, .01, .01));
   //}

   //// Attach our visible object to our root component. Offset and rotate the camera.
   VisualMesh->SetupAttachment(RootComponent);

   springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
   springArm->SetupAttachment(RootComponent);
   springArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 300.f), FRotator(0.0f, 0.0f, 0.0f));
   springArm->TargetArmLength = 0;
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
      FVector NewLocation = GetActorLocation() + (moveInput * DeltaTime);
      SetActorLocation(NewLocation);
      moveInput *= .2;
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

   FVector direction = GetActorRotation().Vector() * GetActorRotation().Vector().Size();
   moveInput += direction * FMath::Clamp(value, -1.0f, 1.0f) * maxSpeed;
}

void APlayerPawn::Move_YAxis(float value)
{
   FVector direction = GetActorRotation().Vector() * GetActorRotation().Vector().Size();
   float temp = direction.X;
   direction.X = -direction.Y;
   direction.Y = temp;
   moveInput += direction * FMath::Clamp(value, -1.0f, 1.0f) * maxSpeed;
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "Boid.h"
#include "ShopKeep.h"
#include "BaitManager.h"
#include "MyPawnMovementComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Inputcomponent.h"
#include "DrawDebugHelpers.h"
#include "UI/Primary_HUD.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

   // Set this pawn to be controlled by the lowest-numbered player
   // AutoPossessPlayer = EAutoReceiveInput::Player0;

   // Create collision object and set as root
   collisionMesh = CreateDefaultSubobject<UCapsuleComponent>(TEXT("collisionMesh"));
   RootComponent = collisionMesh;
   collisionMesh->InitCapsuleSize(60, 160);
   collisionMesh->SetCollisionProfileName(TEXT("Player"));
   //collisionMesh->SetSimulatePhysics(true);
   collisionMesh->BodyInstance.bLockXRotation = true;
   collisionMesh->BodyInstance.bLockYRotation = true;
   collisionMesh->BodyInstance.bLockZRotation = true;

   springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
   springArm->SetupAttachment(RootComponent);
   springArm->SetRelativeLocationAndRotation(FVector(-60.0f, 0.0f, 140.f), FRotator(0.0f, 0.0f, 0.0f));
   springArm->TargetArmLength = -60;
   springArm->bEnableCameraLag = false;
   camera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
   camera->SetupAttachment(springArm, USpringArmComponent::SocketName);

   movementComponent = CreateDefaultSubobject<UMyPawnMovementComponent>(TEXT("CustomMovementComponent"));
   movementComponent->UpdatedComponent = RootComponent;
}

UPawnMovementComponent* APlayerPawn::GetMovementComponent() const
{
   return movementComponent;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
   primaryHUD = Cast<APrimary_HUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);   

   // check out of bounds
   if (GetActorLocation().Z < -fallCutoffHeight)
   {
      UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
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
   InputComponent->BindAction("Interact", IE_Pressed, this, &APlayerPawn::Interact);
   InputComponent->BindAction("PlaceBait", IE_Pressed, this, &APlayerPawn::PlaceBait);
   
   // Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
   InputComponent->BindAxis("XMove", this, &APlayerPawn::Move_XAxis);
   InputComponent->BindAxis("YMove", this, &APlayerPawn::Move_YAxis);

   InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPawn::Jump);

   // Set up "look" bindings.
   PlayerInputComponent->BindAxis("Turn", this, &APlayerPawn::CameraMoveX);
   PlayerInputComponent->BindAxis("LookUp", this, &APlayerPawn::CameraMoveY);
}

void APlayerPawn::Move_XAxis(float value)
{
   if (movementComponent && movementComponent->UpdatedComponent == RootComponent)
   {
      movementComponent->AddInputVector(GetActorForwardVector() * FMath::Clamp(value, -1.0f, 1.0f));
   }
}

void APlayerPawn::Move_YAxis(float value)
{
   if (movementComponent && movementComponent->UpdatedComponent == RootComponent)
   {
      movementComponent->AddInputVector(GetActorRightVector() * FMath::Clamp(value, -1.0f, 1.0f));
   }
}

void APlayerPawn::Jump()
{
   if (!bjumping)
   {
      // if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Jump!")));
      collisionMesh->AddImpulse(GetActorUpVector() * jumpImpulse);
      bjumping = true;
   }
}

void APlayerPawn::CameraMoveX(float value)
{
   mouseInput.X = value;
}

void APlayerPawn::CameraMoveY(float value)
{
   mouseInput.Y = -value;
}

void APlayerPawn::Interact()
{
   FHitResult outHit = this->TraceCollision(reachDistance);

   if (outHit.bBlockingHit && outHit.GetActor()->IsA(ABoid::StaticClass()))
   {
      ++numFish;
      if (primaryHUD)
      {
         primaryHUD->FishValueMore(numFish);
      }
      if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Caught Fish!")));

      ((ABoid*)outHit.GetActor())->Enter();
   }
   else if (outHit.bBlockingHit && outHit.GetActor()->IsA(AShopKeep::StaticClass()))
   {
      if (numFish >= 3)
      {
         ++numBait;
         if (primaryHUD)
         {
            primaryHUD->BaitValueMore(numBait);
         }
         numFish -= 3;
         if (primaryHUD)
         {
            primaryHUD->FishValueLess(numFish);
         }
         if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Bought Bait!")));
      }
   }
}

void APlayerPawn::PlaceBait()
{
   FHitResult outHit = this->TraceCollision(reachDistance * 2);
   if (numBait > 0)
   {
      if (outHit.bBlockingHit && *outHit.GetActor()->GetName() == FString("Floor_39"))
      {
         baitManager->SpawnBait(outHit.ImpactPoint);
         --numBait;
         if (primaryHUD)
         {
            primaryHUD->BaitValueLess(numBait);
         }
      }
   }
}

FHitResult APlayerPawn::TraceCollision(float dist)
{
   FHitResult outHit;
   FVector start = camera->GetComponentLocation();
   FVector forwardVector = camera->GetForwardVector();
   FVector end = (forwardVector * dist) + start;

   FCollisionQueryParams collisionParams;
   bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collisionParams);

   // if (isHit)
   // {
   //    if (outHit.bBlockingHit)
   //    {
   //       if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *outHit.GetActor()->GetName()));
   //    }
   // }
   return outHit;

   // DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 1, 0, 1);
}

void APlayerPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
   Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
   // if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s at %d"), *Other->GetName(), HitNormal.Z));
   if (HitNormal.Z > 0)
   {
      bjumping = false;
   }
}
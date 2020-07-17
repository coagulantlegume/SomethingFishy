// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopKeep.h"
#include "PlayerPawn.h"

#include "Components/BoxComponent.h"

// Sets default values
AShopKeep::AShopKeep()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

   // Set up visual mesh
   this->VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
   RootComponent = VisualMesh;

   static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/LPAnimals/Meshes/Turtle.Turtle"));

   if (CubeVisualAsset.Succeeded())
   {
      this->VisualMesh->SetStaticMesh(CubeVisualAsset.Object);
   }
}

// Called when the game starts or when spawned
void AShopKeep::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShopKeep::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShopKeep::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
   if (OtherActor->IsA(APlayerPawn::StaticClass()))
      if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Bought Bait!")));
}

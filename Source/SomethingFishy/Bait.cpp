// Fill out your copyright notice in the Description page of Project Settings.


#include "Bait.h"

// Sets default values
ABait::ABait()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

   // Set up visual mesh
   this->VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
   this->VisualMesh->SetupAttachment(RootComponent);

   static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));

   if (CubeVisualAsset.Succeeded())
   {
      this->VisualMesh->SetStaticMesh(CubeVisualAsset.Object);
      // this->VisualMesh->SetRelativeLocation(spawnLocation);
      this->VisualMesh->SetRelativeScale3D(FVector(this->value / 32, this->value / 32, this->value / 32));
   }
}

// Called when the game starts or when spawned
void ABait::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABait::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABait::Bite()
{
   this->value -= 1;
   this->VisualMesh->SetRelativeScale3D(FVector(this->value / 32, this->value / 32, this->value / 32));
}

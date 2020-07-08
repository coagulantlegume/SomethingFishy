// Fill out your copyright notice in the Description page of Project Settings.


#include "Arms.h"
#include "Boid.h"

// Sets default values
AArms::AArms()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set up visual mesh
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	VisualMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));

	if (CubeVisualAsset.Succeeded())
	{
		this->VisualMesh->SetStaticMesh(CubeVisualAsset.Object);
		this->VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		this->VisualMesh->SetRelativeScale3D(FVector(1, 1, .1));
	}
}

// Called when the game starts or when spawned
void AArms::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArms::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArms::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
   Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
   // if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *Other->GetName()));
   if (Other->IsA(ABoid::StaticClass()))
   {
      if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Caught Fish!")));
		((ABoid*)Other)->Remove();
   }
}

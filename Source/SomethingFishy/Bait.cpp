// Fill out your copyright notice in the Description page of Project Settings.


#include "Bait.h"
#include "Boid.h"

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
      this->VisualMesh->SetRelativeScale3D(FVector(this->value / 32 + .2, this->value / 32 + .2, this->value / 32 + .2));
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

void ABait::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
   Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
   // if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *Other->GetName()));
   if (Other->IsA(ABoid::StaticClass()))
   {
     // if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Bitten!")));
     --this->value;
     if (this->value > 0)
     {
        this->VisualMesh->SetRelativeScale3D(FVector(this->value / 32 + .2, this->value / 32 + .2, this->value / 32 + .2));
        // if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Size: %f, %d"), this->GetActorScale().X, this->value));
     }
     else
     {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Bait gone!")));
        this->ConditionalBeginDestroy();
     }
   }
}

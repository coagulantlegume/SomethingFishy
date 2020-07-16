// Fill out your copyright notice in the Description page of Project Settings.


#include "Bait.h"
#include "Boid.h"
#include "BaitManager.h"

// Sets default values
ABait::ABait()
{
   // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
   PrimaryActorTick.bCanEverTick = true;

   // Set up visual mesh
   this->VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
   this->VisualMesh->SetupAttachment(RootComponent);

   static ConstructorHelpers::FObjectFinder<UStaticMesh> VisualAsset(TEXT("/Game/Bait/bait.bait"));

   if (VisualAsset.Succeeded())
   {
      this->VisualMesh->SetStaticMesh(VisualAsset.Object);
      // this->VisualMesh->SetRelativeLocation(spawnLocation);
      this->VisualMesh->SetRelativeScale3D(FVector(this->value * 128 / 32 + 5, this->value * 128 / 32 + 5, this->value * 128 / 32 + 5));
   }
   
   // set starting value
   value = 16;
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
         this->VisualMesh->SetRelativeScale3D(FVector(this->value * 128 / 32 + 5, this->value * 128 / 32 + 5, this->value * 128 / 32 + 5));
      }
      else
      {
         if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Bait gone!")));
         this->ConditionalBeginDestroy();
      }
   }
}

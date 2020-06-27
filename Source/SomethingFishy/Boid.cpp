// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"
#include "Flock.h"

// Sets default values
ABoid::ABoid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	VisualMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

	if (SphereVisualAsset.Succeeded())
	{
		VisualMesh->SetStaticMesh(SphereVisualAsset.Object);
		VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}

	SetActorScale3D(GetActorScale() * .5);
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	std::vector<ABoid*> curr_flockmates;
	//this->myFlock->AFlock::GetVisibleFlockmates(this, this->perceptionRange, curr_flockmates);

}

// Separation: Steer to avoid crowding local flockmates
void ABoid::Separation(const std::vector<ABoid*>& flockMates)
{

}

// Alignment: Steer towards the average heading of local flockmates
void ABoid::Alignment(const std::vector<ABoid*>& flockMates)
{

}

// Cohesion: Steer to move toward the average position of local flockmates
void ABoid::Cohesion(const std::vector<ABoid*>& flockMates)
{

}

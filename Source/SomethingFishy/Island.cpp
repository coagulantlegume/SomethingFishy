// Fill out your copyright notice in the Description page of Project Settings.


#include "Island.h"

#include "ProceduralMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "FastNoiseWrapper.h"
#include <math.h>

// Sets default values
AIsland::AIsland()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create mesh object
	CustomMesh = CreateDefaultSubobject<UProceduralMeshComponent>("CustomMesh");
	SetRootComponent(CustomMesh);
	CustomMesh->bUseAsyncCooking = true;

	// Create noise object
	TerrainNoise = CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("FastNoiseWrapper"));
	// TerrainNoise->SetupFastNoise();
}

// Called when the game starts or when spawned
void AIsland::BeginPlay()
{
	Super::BeginPlay();

	GeneratePlane();
}

void AIsland::AddTriangle(int32 V1, int32 V2, int32 V3)
{
	Triangles.Add(V1);
	Triangles.Add(V2);
	Triangles.Add(V3);
}

void AIsland::GeneratePlane()
{
	for (int x = 0; x <= dimensions.X; ++x) // create initial line
	{
		Vertices.Add(tiltToReal(FVector(x * unitSize, 0, FMath::RandRange(-dimensions.Z / 2, dimensions.Z / 2))));// FMath::PerlinNoise2D(FVector2D(x * unitSize, 0)) * dimensions.Z)));
	}

	for (int y = 0; y < dimensions.Y; ++y)
	{
		Vertices.Add(tiltToReal(FVector(0, (y + 1) * unitSize, FMath::RandRange(-dimensions.Z / 2, dimensions.Z / 2)))); // Add upper left vertex of new row
		for (int x = 0; x < dimensions.X; ++x)
		{
			// Add upper right vertex of new square
			Vertices.Add(tiltToReal(FVector((x + 1) * unitSize, (y + 1) * unitSize, FMath::RandRange(-dimensions.Z / 2, dimensions.Z / 2)))); // FMath::PerlinNoise2D(FVector2D((x + 1) * unitSize, (y + 1) * unitSize)) * dimensions.Z)));
			// Add lower triangle
			AddTriangle((dimensions.X + 1) * y + x, (dimensions.X + 1) * (y + 1) + (x), (dimensions.X + 1) * y + x + 1);
			// Add upper triangle
			AddTriangle((dimensions.X + 1) * y + x + 1, (dimensions.X + 1) * (y + 1) + (x), (dimensions.X + 1) * (y + 1) + (x + 1));
		}
	}

	TArray<FLinearColor> VertexColors;
	for (int i = 0; i < Vertices.Num(); ++i)
	{
		VertexColors.Add(FLinearColor(0.f, 0.f, 0.f));
	}

	CustomMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), VertexColors, TArray<FProcMeshTangent>(), true);
}

// Called every frame
void AIsland::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AIsland::tiltToReal(FVector tilt)
{
	return tilt.X * FVector(1, 0, 0) + tilt.Y * FVector(0.5, sqrt(3) / 2, 0) + tilt.Z * FVector(0, 0, 1);
}
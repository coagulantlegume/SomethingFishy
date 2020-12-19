// Fill out your copyright notice in the Description page of Project Settings.


#include "Island.h"

#include "ProceduralMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include <cmath>

// Sets default values
AIsland::AIsland()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create mesh object
	CustomMesh = CreateDefaultSubobject<UProceduralMeshComponent>("CustomMesh");
	SetRootComponent(CustomMesh);
	CustomMesh->bUseAsyncCooking = true;
}

// Called when an instance of this class is placed or spawned (in editor)
void AIsland::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Hex top/bottom variables
	xOffset = sqrt(3) * unitSize;
	yOffset = (3 * unitSize) / 2;

	GeneratePlane();
}

// Called when the game starts or when spawned
void AIsland::BeginPlay()
{
	Super::BeginPlay();
}

void AIsland::AddTriangle(int32 V1, int32 V2, int32 V3)
{
	Triangles.Add(V1);
	Triangles.Add(V2);
	Triangles.Add(V3);
}

void AIsland::GeneratePlane()
{
	drawHex(FVector(0, 0, 0));

	// for (int x = 0; x <= dimensions.X; ++x) // create initial line
	// {
	// 	Vertices.Add(FVector(x * unitSize, 0, 0));
	// }
	// 
	// for (int y = 0; y < dimensions.Y; ++y)
	// {
	// 	Vertices.Add(FVector(0, (y + 1) * unitSize, 0)); // Add upper left vertex of new row
	// 	for (int x = 0; x < dimensions.X; ++x)
	// 	{
	// 		// Add upper right vertex of new square
	// 		Vertices.Add(FVector((x + 1) * unitSize, (y + 1) * unitSize, 0));
	// 		// Add lower triangle
	// 		AddTriangle((dimensions.X + 1) * y + x, (dimensions.X + 1) * (y + 1) + (x), (dimensions.X + 1) * y + x + 1);
	// 		// Add upper triangle
	// 		AddTriangle((dimensions.X + 1) * y + x + 1, (dimensions.X + 1) * (y + 1) + (x), (dimensions.X + 1) * (y + 1) + (x + 1));
	// 	}
	// }

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

FVector AIsland::hexToWorld(FVector loc)
{
	return FVector(loc.X * xOffset + ((int)loc.Y % 2) * (xOffset / 2), loc.Y * yOffset, 0);
}

void AIsland::drawHex(FVector loc)
{
	FVector origin = hexToWorld(loc);
	// center point
	int origin_index = Vertices.Num();
	Vertices.Add(origin);
	hexPoints[origin] = origin_index;

	// 12:00
	Vertices.Add(FVector(origin.X, origin.Y + unitSize, 0));

	// 2:00
	Vertices.Add(FVector(origin.X + xOffset / 2, origin.Y + unitSize / 2, 0));

	// 4:00
	Vertices.Add(FVector(origin.X + xOffset / 2, origin.Y - unitSize / 2, 0));

	// 6:00
	Vertices.Add(FVector(origin.X, origin.Y - unitSize, 0));

	// 8:00
	Vertices.Add(FVector(origin.X - xOffset / 2, origin.Y - unitSize / 2, 0));

	// 10:00
	Vertices.Add(FVector(origin.X - xOffset / 2, origin.Y + unitSize / 2, 0));

	// Connect triangles
	for (int i = origin_index + 1; i < origin_index + 6; ++i)
	{
		AddTriangle(i, i + 1, origin_index);
	}
	AddTriangle(origin_index + 6, origin_index + 1, 0);
}

void AIsland::getNeighbors(FVector loc, std::vector<FVector> &neighbors)
{
	neighbors.clear();
	neighbors.resize(6);

	FVector possibleNeighbor;

	// 1:00
	possibleNeighbor = FVector(loc.X + ((int)loc.X % 2), loc.Y + 1, loc.Z);
	if (hexPoints.find(possibleNeighbor) != hexPoints.end())
	{
		neighbors[0] = possibleNeighbor;
	}

	// 3:00
	possibleNeighbor = FVector(loc.X + 1, loc.Y, loc.Z);
	if (hexPoints.find(possibleNeighbor) != hexPoints.end())
	{
		neighbors[1] = possibleNeighbor;
	}

	// 5:00
	possibleNeighbor = FVector(loc.X + ((int)loc.X % 2), loc.Y - 1, loc.Z);
	if (hexPoints.find(possibleNeighbor) != hexPoints.end())
	{
		neighbors[2] = possibleNeighbor;
	}

	// 7:00
	possibleNeighbor = FVector(loc.X - (((int)loc.X + 1) % 2), loc.Y - 1, loc.Z);
	if (hexPoints.find(possibleNeighbor) != hexPoints.end())
	{
		neighbors[3] = possibleNeighbor;
	}

	// 9:00
	possibleNeighbor = FVector(loc.X - 1, loc.Y, loc.Z);
	if (hexPoints.find(possibleNeighbor) != hexPoints.end())
	{
		neighbors[4] = possibleNeighbor;
	}

	// 11:00
	possibleNeighbor = FVector(loc.X - (((int)loc.X + 1) % 2), loc.Y + 1, loc.Z);
	if (hexPoints.find(possibleNeighbor) != hexPoints.end())
	{
		neighbors[5] = possibleNeighbor;
	}
}

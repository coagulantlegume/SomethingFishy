// Fill out your copyright notice in the Description page of Project Settings.


#include "Island.h"

#include "ProceduralMeshComponent.h"
#include "FastNoiseLite.h"
#include "Math/UnrealMathUtility.h"
#include <cmath>
#include <cstdlib>

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

	Vertices.Empty();
	Triangles.Empty();
	triPoint.clear();

	// Hex top/bottom variables
	xOffset = sqrt(3) * unitSize;
	yOffset = (3 * unitSize) / 2;

	// Create island footprint
	// growIsland();

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
	// Setup noise generators
	FastNoiseLite topNoiseGen1;
	topNoiseGen1.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	topNoiseGen1.SetSeed(TopNoiseSeed1);

	FastNoiseLite topNoiseGen2;
	topNoiseGen2.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	topNoiseGen2.SetSeed(TopNoiseSeed2);

	// First pass (structure tris, no overlap vertices)
	// for (int y = 0; y < dimensions.Y; y += 2)
	// {
	// 	for (int x = y % 2; x < dimensions.X; x += 3)
	// 	{
	// 		drawStructureTri(FVector(x, y, 1));
	// 	}
	// }

	// Second pass (fill tris, mostly overlap vertices)
	for (int y = 0; y < dimensions.Y; ++y)
	{
		for (int x = 0; x < dimensions.X; ++x)
		{
			// if ((x + 3 - (y % 2)) % 3) // not already drawn structure hex
			// {
				drawFillTri(FVector(x, y, 1));
			// }
		}
	}

	TArray<FLinearColor> VertexColors;
	for (int i = 0; i < Vertices.Num(); ++i)
	{
		VertexColors.Add(FLinearColor(0.f, 0.f, 0.f));

		// Set height with noise
		float height = topNoiseGen1.GetNoise(Vertices[i].X * TopNoiseScale1, Vertices[i].Y * TopNoiseScale1) * TopNoiseWeight1;
		height += topNoiseGen2.GetNoise(Vertices[i].X * TopNoiseScale2, Vertices[i].Y * TopNoiseScale2) * TopNoiseWeight2;
		height += TopNoiseWeight1 + TopNoiseWeight2; // to make all positive
		height /= TopNoiseWeight1 + TopNoiseWeight2; // make 0-1
		height *= TopHeight; // scale to specified
		Vertices[i].Z = height;
	}

	CustomMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), VertexColors, TArray<FProcMeshTangent>(), true);
}

// Called every frame
void AIsland::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIsland::drawStructureTri(FVector loc)
{
	// setup for first point added to be recorded as direction point
	int point_index = Vertices.Num();
	triPoint[loc] = Triangles.Num();

	for (int i = 0; i < 3; ++i)
	{
		Vertices.Add(getCornerCoordinate(loc, i));
	}

	// Connect
	AddTriangle(point_index, point_index + 1, point_index + 2);
}

void AIsland::drawFillTri(FVector loc)
{
	triPoint[loc] = Triangles.Num();

	// find all neighbors for shared points
	std::vector<FVector> neighbors;
	getNeighbors(loc, neighbors);
	
	std::vector<int> points(3, -1); // indices of vertices in Vertices

	// fill edge points with index of existing vertices
	for (int i = 0; i < 3; ++i)
	{
		if (neighbors[i] != FVector::ZeroVector)
		{
			points[(i + 1) % 3] = Triangles[triPoint[neighbors[i]] + (i + 2) % 3];
			points[(i + 2) % 3] = Triangles[triPoint[neighbors[i]] + (i + 1) % 3];
		}
	}

	// Check missing vertices for existing tris with only one shared vertex
	for (int i = 0; i < 3; ++i)
	{
		if (points[i] == -1)
		{
			getTangentNeighbors(loc, i, neighbors);
			for (int j = 0; j < 3 && points[i] == -1; ++j)
			{
				if (neighbors[j] != FVector::ZeroVector)
				{
					points[i] = Triangles[triPoint[neighbors[j]] + (i + 2 + (2 - j)) % 3];
				}
			}
		}
	}

	// make missing vertices and add indices to masterVert
	for (int i = 0; i < 3; ++i)
	{
		if (points[i] == -1) // no existing vert found
		{
			points[i] = Vertices.Num();
			Vertices.Add(getCornerCoordinate(loc, i));
		}
	}

	AddTriangle(points[0], points[1], points[2]);
}

// pos 0 = up/down direction point, 1 = clockwise from direction point, 2 = clockwise from 1
FVector AIsland::getCornerCoordinate(FVector loc, int pos)
{
	int dirOffset = ((int)loc.X + (int)loc.Y) % 2 * 2 - 1; // 1 if pointing up, -1 if pointing down.
	FVector vertLoc = triToWorld(loc);

	switch (pos)
	{
	case 0:
		vertLoc.Y += unitSize * (sqrt(3) / 4) * dirOffset;
		break;
	case 1:
		vertLoc.Y -= unitSize * (sqrt(3) / 4) * dirOffset;
		vertLoc.X += unitSize / 2 * dirOffset;
		break;
	case 2:
		vertLoc.Y -= unitSize * (sqrt(3) / 4) * dirOffset;
		vertLoc.X -= unitSize / 2 * dirOffset;
		break;
	default:
		vertLoc = FVector::ZeroVector;
	}
	return vertLoc;
}

void AIsland::getNeighbors(FVector loc, std::vector<FVector> &neighbors)
{
	neighbors.clear();
	neighbors.resize(3, FVector::ZeroVector);

	std::vector<FVector> tileNeighbor;
	getNeighborTiles(loc, tileNeighbor);

	for (int i = 0; i < 3; ++i)
	{
		if (triPoint.find(tileNeighbor[i]) != triPoint.end())
		{
			neighbors[i] = tileNeighbor[i];
		}
	}
}

void AIsland::getNeighborTiles(FVector loc, std::vector<FVector>& neighbors)
{
	neighbors.clear();
	neighbors.resize(3);
	int dirOffset = ((int)loc.X + (int)loc.Y) % 2 * 2 - 1; // 1 if pointing up, -1 if pointing down.

	neighbors[0] = FVector(loc.X, loc.Y - dirOffset, loc.Z);
	neighbors[1] = FVector(loc.X - dirOffset, loc.Y, loc.Z);
	neighbors[2] = FVector(loc.X + dirOffset, loc.Y, loc.Z);
}

// pos 0 = up/down direction point, 1 = clockwise from direction point, 2 = clockwise from 1
void AIsland::getTangentNeighbors(FVector loc, int pos, std::vector<FVector>& neighbors)
{
	neighbors.clear();
	neighbors.resize(3);
	int dirOffset = ((int)loc.X + (int)loc.Y) % 2 * 2 - 1; // 1 if pointing up, -1 if pointing down.

	FVector possibleNeighbors[3];

	switch (pos)
	{
	case 0:
		possibleNeighbors[0] = FVector(loc.X - 1 * dirOffset, loc.Y + 1 * dirOffset, loc.Z);
		possibleNeighbors[1] = FVector(loc.X + 0 * dirOffset, loc.Y + 1 * dirOffset, loc.Z);
		possibleNeighbors[2] = FVector(loc.X + 1 * dirOffset, loc.Y + 1 * dirOffset, loc.Z);
		break;
	case 1:
		possibleNeighbors[0] = FVector(loc.X + 2 * dirOffset, loc.Y + 0 * dirOffset, loc.Z);
		possibleNeighbors[1] = FVector(loc.X + 2 * dirOffset, loc.Y - 1 * dirOffset, loc.Z);
		possibleNeighbors[2] = FVector(loc.X + 1 * dirOffset, loc.Y - 1 * dirOffset, loc.Z);
		break;
	case 2:
		possibleNeighbors[0] = FVector(loc.X - 1 * dirOffset, loc.Y - 1 * dirOffset, loc.Z);
		possibleNeighbors[1] = FVector(loc.X - 2 * dirOffset, loc.Y - 1 * dirOffset, loc.Z);
		possibleNeighbors[2] = FVector(loc.X - 2 * dirOffset, loc.Y + 0 * dirOffset, loc.Z);
		break;
	default:
		possibleNeighbors[0] = FVector::ZeroVector;
		possibleNeighbors[1] = FVector::ZeroVector;
		possibleNeighbors[2] = FVector::ZeroVector;
	}

	// Check if neighbor existing tile, and add to neighbors if it is
	for (int i = 0; i < 3; ++i)
	{
		if (triPoint.find(possibleNeighbors[i]) != triPoint.end())
		{
			neighbors[i] = possibleNeighbors[i];
		}
		else
		{
			neighbors[i] = FVector::ZeroVector;
		}
	}
}

FVector AIsland::triToWorld(FVector loc)
{
	return FVector((loc.X + 1) * (unitSize / 2), (loc.Y + 0.5) * (unitSize * (sqrt(3) / 2)), 0);
}

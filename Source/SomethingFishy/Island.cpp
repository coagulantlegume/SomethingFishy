// Fill out your copyright notice in the Description page of Project Settings.


#include "Island.h"

#include "ProceduralMeshComponent.h"
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

	// Hex top/bottom variables
	xOffset = sqrt(3) * unitSize;
	yOffset = (3 * unitSize) / 2;

	// Create island footprint
	growIsland();

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
	// First pass (structure hexes, no overlap vertices)
	for (int y = 0; y < dimensions.Y; ++y)
	{
		for (int x = y % 2; x < dimensions.X; x += 3)
		{
			drawStructureHex(FVector(x, y, 1));
		}
	}

	fillStart = Vertices.Num();

	// Second pass (fill hexes, mostly overlap vertices)
	for (int y = 0; y < dimensions.Y; ++y)
	{
		for (int x = 0; x < dimensions.X; ++x)
		{
			if ((x + 3 - (y % 2)) % 3) // not already drawn structure hex
			{
				drawFillHex(FVector(x, y, 1));
			}
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

FVector AIsland::hexToWorld(FVector loc)
{
	return FVector(loc.X * xOffset + ((int)loc.Y % 2) * (xOffset / 2), loc.Y * yOffset, 0);
}

void AIsland::drawStructureHex(FVector loc)
{
	FVector origin = hexToWorld(loc);
	// center point
	int origin_index = Vertices.Num();
	Vertices.Add(origin);
	hexPoints[loc] = origin_index;

	for (int i = 0; i < 6; ++i)
	{
		Vertices.Add(getCornerCoordinate(origin, i));
	}

	// Connect triangles
	for (int i = 1; i < 7; ++i)
	{
		AddTriangle(origin_index + i, origin_index + i % 6 + 1, origin_index);
	}
}

void AIsland::drawFillHex(FVector loc)
{
	FVector origin = hexToWorld(loc);
	// center point
	int origin_index = Vertices.Num();
	Vertices.Add(origin);
	hexPoints[loc] = origin_index;

	// find all neighbors for shared points
	std::vector<FVector> neighbors;
	getNeighbors(loc, neighbors);

	std::vector<int> edgePoints(6, -1);

	// fill edge points with index of existing structure hex vertices
	for (int i = 0; i < 6; ++i)
	{
		if (neighbors[i] != FVector::ZeroVector)
		{
			auto it = hexPoints.find(neighbors[i]);
			if (it != hexPoints.end() && it->second < fillStart) // structure
			{
				edgePoints[i] = it->second + (i + 4) % 6 + 1;
				edgePoints[(i + 1) % 6] = it->second + (i + 3) % 6 + 1;
			}
			else if (it != hexPoints.end() && it->second >= fillStart) // fill
			{
				for (int pos = it->second + 1; pos < 6; ++i) // TODO: narrow search more
				{
					if (Vertices[pos] == getCornerCoordinate(origin, i))
					{
						edgePoints[i] = pos;
					}
					else if (Vertices[pos] == getCornerCoordinate(origin, (i + 1) % 6))
					{
						edgePoints[(i + 1) % 6] = pos;
					}
				}
			}
		}
	}

	// add vertices which did not already exist
	for (int i = 0; i < 6; ++i)
	{
		if (edgePoints[i] == -1)
		{
			edgePoints[i] = Vertices.Num();
			Vertices.Add(getCornerCoordinate(origin, i));
		}
	}

	// Connect triangles
	for (int i = 0; i < 6; ++i)
	{
		AddTriangle(edgePoints[i], edgePoints[(i + 1) % 6], origin_index);
	}
}

FVector AIsland::getCornerCoordinate(FVector origin, int pos)
{
	switch (pos)
	{
		case 0:
			// 12:00
			return FVector(origin.X, origin.Y + unitSize, 0);
		case 1:
			// 2:00
			return FVector(origin.X + xOffset / 2, origin.Y + unitSize / 2, 0);
		case 2:
			// 4:00
			return FVector(origin.X + xOffset / 2, origin.Y - unitSize / 2, 0);
		case 3:
			// 6:00
			return FVector(origin.X, origin.Y - unitSize, 0);
		case 4:
			// 8:00
			return FVector(origin.X - xOffset / 2, origin.Y - unitSize / 2, 0);
		case 5:
			// 10:00
			return FVector(origin.X - xOffset / 2, origin.Y + unitSize / 2, 0);
		default:
			return FVector(0, 0, 0);
	}
}

void AIsland::getNeighbors(FVector loc, std::vector<FVector> &neighbors)
{
	neighbors.clear();
	neighbors.resize(6, FVector::ZeroVector);

	FVector possibleNeighbor;

	// 1:00
	possibleNeighbor = FVector(loc.X + (((int)loc.Y) % 2), loc.Y + 1, loc.Z);
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
	possibleNeighbor = FVector(loc.X + (((int)loc.Y) % 2), loc.Y - 1, loc.Z);
	if (hexPoints.find(possibleNeighbor) != hexPoints.end())
	{
		neighbors[2] = possibleNeighbor;
	}

	// 7:00
	possibleNeighbor = FVector(loc.X - (((int)loc.Y + 1) % 2), loc.Y - 1, loc.Z);
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
	possibleNeighbor = FVector(loc.X - (((int)loc.Y + 1) % 2), loc.Y + 1, loc.Z);
	if (hexPoints.find(possibleNeighbor) != hexPoints.end())
	{
		neighbors[5] = possibleNeighbor;
	}
}

void AIsland::getNeighborTiles(FIntPoint loc, std::vector<FIntPoint>& neighbors)
{
	neighbors.clear();
	neighbors.resize(6);

	// 1:00
	neighbors[0] = FIntPoint(loc.X + (((int)loc.Y) % 2), loc.Y + 1);

	// 3:00
	neighbors[1] = FIntPoint(loc.X + 1, loc.Y);

	// 5:00
	neighbors[2] = FIntPoint(loc.X + (((int)loc.Y) % 2), loc.Y - 1);

	// 7:00
	neighbors[3] = FIntPoint(loc.X - (((int)loc.Y + 1) % 2), loc.Y - 1);

	// 9:00
	neighbors[4] = FIntPoint(loc.X - 1, loc.Y);

	// 11:00
	neighbors[5] = FIntPoint(loc.X - (((int)loc.Y + 1) % 2), loc.Y + 1);
}

// populates island grid
void AIsland::growIsland()
{
	islandGrid.SetNum(dimensions.X);
	for (int x = 0; x < islandGrid.Num(); ++x)
	{
		islandGrid[x].SetNum(dimensions.Y);
	}

	islandGrid[(int)dimensions.X / 2][(int)dimensions.Y / 2] = true; // seed point

	bool edgeReached = false;
	std::vector<FIntPoint> neighbors;
	edgePieces.push_back(FIntPoint(dimensions.X / 2, dimensions.Y / 2));
	
	// grow edge pieces until one reaches bound
	while (!edgeReached)
	{
		for (auto it = edgePieces.begin(); it != edgePieces.end();)
		{
			// float dist = FVector::Dist(hexToWorld(FVector(it->X, it->Y, 1)), hexToWorld(FVector(dimensions.X, dimensions.Y, 1))) / dimensions.X;
			getNeighborTiles(*it, neighbors);
			bool surrounded = true;
	
			for (int j = 0; j < 6; ++j)
			{
				if (neighbors[j].X < 0 || neighbors[j].X >= dimensions.X ||
					 neighbors[j].Y < 0 || neighbors[j].Y >= dimensions.Y)
				{
					edgeReached = true;
				}
				else if (!islandGrid[neighbors[j].X][neighbors[j].Y] && (rand() % 2))
				{
					islandGrid[neighbors[j].X][neighbors[j].Y] = true;
					edgePieces.push_front(FIntPoint(neighbors[j].X, neighbors[j].Y));
				}
				else if (!islandGrid[neighbors[j].X][neighbors[j].Y])
				{
					surrounded = false;
				}
			}
	
			if (surrounded)
			{
				edgePieces.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
}

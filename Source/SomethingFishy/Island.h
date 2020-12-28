// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Vector3Map.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Island.generated.h"

UCLASS()
class SOMETHINGFISHY_API AIsland : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIsland();

protected:
	// Called when an instance of this class is placed or spawned (in editor)
	virtual void OnConstruction(const FTransform &Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(editAnywhere)
	class UProceduralMeshComponent* CustomMesh;

	UPROPERTY(editAnywhere)
	FVector dimensions = FVector(100, 100, 100);

	UPROPERTY(editAnywhere)
		float TopHeight;

	UPROPERTY(editAnywhere)
		int TopNoiseSeed1;
	UPROPERTY(editAnywhere)
		int TopNoiseSeed2;

	UPROPERTY(editAnywhere)
		int TopNoiseWeight1;
	UPROPERTY(editAnywhere)
		int TopNoiseWeight2;

	UPROPERTY(editAnywhere)
		float TopNoiseScale1;
	UPROPERTY(editAnywhere)
		float TopNoiseScale2;

	UPROPERTY(editAnywhere)
	float unitSize = 1;

	float xOffset;
	float yOffset;

	TArray<TArray<bool>> islandGrid;
	std::list<FIntPoint> edgePieces;

	TArray<FVector> Vertices;
	TArray<int32> Triangles;

	std::map<FVector, int, FVectorCmp> triPoint; // Up/down vertex of given tri as index in Triangles

	void AddTriangle(int32 V1, int32 V2, int32 V3);
	void GeneratePlane();
	void GenerateTriIsland();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void drawStructureTri(FVector loc);
	void drawFillTri(FVector loc);

	FVector getCornerCoordinate(FVector loc, int pos);

	void getNeighbors(FVector loc, std::vector<FVector> &neighbors); // Returns all hex neighbors that have been constructed
	void getNeighborTiles(FVector loc, std::vector<FVector> &neighbors); // Returns all coordinates surrounding given coordinate
	void getTangentNeighbors(FVector loc, int pos, std::vector<FVector>& neighbors); // Returns all triangles sharing only vertex at specified pos with loc

	FVector triToWorld(FVector loc);

	void growIsland();
};

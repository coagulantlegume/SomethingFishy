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
	float unitSize = 1;

	float xOffset;
	float yOffset;

	TArray<FVector> Vertices;
	TArray<int32> Triangles;

	std::map<FVector, int, FVectorCmp> hexPoints; // Container of hexes drawn, with key = Vector3(hex_x, hex_y, layer (top = 1, bottom = 0)

	void AddTriangle(int32 V1, int32 V2, int32 V3);
	void GeneratePlane();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void drawHex(FVector loc);
	void getNeighbors(FVector loc, std::vector<FVector> &neighbors);

	FVector hexToWorld(FVector loc);
};

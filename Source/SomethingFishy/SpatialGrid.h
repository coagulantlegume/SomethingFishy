// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpatialGrid.generated.h"

/* FVector less than comparison for map use */
struct FVectorCmp2 {
	bool operator()(const FVector& a, const FVector& b) const;
};

UCLASS()
class SOMETHINGFISHY_API ASpatialGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	/****************************************
	*  Basic Public Functions
	*****************************************/
	/* Constructor */
	ASpatialGrid();

	/* Tick */
	virtual void Tick(float DeltaTime) override;

	/* Getters */
	int GetUnitSize();
	int GetActiveCount();
	int GetPassiveCount();
	std::list<AActor*> GetNeighbors(FVector loc); // returns list of all actors in same cell and neighboring cells

	/* Manipulators */
	void setUnitSize(int size); // Set the unit size used for cells
	void MakeStatic(); // Do not update with tick, for objects that do not move in the scene
	void MakeDynamic(); // Update with tick, for objects that move in the scene

	FVector AddElement(AActor* elem); // Adds new element reference to grid, returning the grid coordinate
	void RemoveElement(AActor* elem); // Removes specified element from grid
	void ClearPassive(); // Frees memory from cells with no elements

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/****************************************
	*  Cell Nested Class
	*****************************************/
	class Cell
	{
	public:
		/*************************************
		*  Basic Public Functions
		**************************************/
		/* Constructor */
		Cell();

		/* Destructor */
		~Cell();

		/* Getters */
		std::list<AActor*> GetElements(); // Returns list of all element pointers
		int GetSize(); // Returns number of entities in cell
		bool isActive(); // Returns whether cell currently has any elements

		/* Manipulators */
		void AddElement(AActor* elem); // Adds new element reference to cell

		void ClearCell(); // Destroys all elements in cell
		bool RemoveElement(AActor* elem); // Removes specified element from cell, returning whether element was found and removed or not

	private:
		/*************************************
		*  Member Variables
		**************************************/
		std::list<AActor*> elements;
	};

	/****************************************
	*  Member Variables
	*****************************************/
	int unitSize; // length of one cell edge
	int activeCount; // number of cells with elements inside
	int passiveCount; // number of cells allocated without elements inside

	std::map<FVector, Cell, FVectorCmp2> grid; // cell container, with Key = unit grid coordinate, Data = cell contents

	/****************************************
	*  Helper Functions
	*****************************************/
	void CleanCell(FVector cell); // Checks bounds and moves any elements outside of bounds to the proper cell
};

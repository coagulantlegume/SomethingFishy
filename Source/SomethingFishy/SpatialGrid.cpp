// Fill out your copyright notice in the Description page of Project Settings.


#include "SpatialGrid.h"
/* FVector less than comparison for map use */
bool FVectorCmp2::operator()(const FVector& a, const FVector& b) const {
	if (a.X < b.X)	return true;
	if (a.X > b.X)	return false;
	if (a.Y < b.Y)	return true;
	if (a.Y > b.Y)	return false;
	if (a.Z < b.Z)	return true;
	if (a.Z > b.Z)	return false;
	return false;
}

/****************************************
*  Basic Public Functions
*****************************************/
ASpatialGrid::ASpatialGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	unitSize = 1;
	activeCount = 0;
	passiveCount = 0;
}

void ASpatialGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	std::map<FVector, Cell, FVectorCmp2>::iterator cell = grid.begin();
	while (cell != grid.end())
	{
		if (cell->second.isActive())
		{
			CleanCell(cell->first);
		}
		++cell;
	}
}

int ASpatialGrid::GetUnitSize()
{
	return unitSize;
}

int ASpatialGrid::GetActiveCount()
{
	return activeCount;
}

int ASpatialGrid::GetPassiveCount()
{
	return passiveCount;
}

// returns list of all actors in same cell and neighboring cells
std::list<AActor*> ASpatialGrid::GetNeighbors(FVector loc)
{
	std::list<AActor*> neighbors;
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			for (int z = -1; z <= 1; ++z)
			{
				std::list<AActor*> newNeighbors = grid[FVector(x, y, z)].GetElements();
				for (const auto& elem : newNeighbors)
				{
					neighbors.push_back(elem);
				}
			}
		}
	}
	return neighbors;
}

// Set the unit size used for cells
void ASpatialGrid::setUnitSize(int size)
{
	ClearPassive();
	if (GetActiveCount())
	{
		// TODO: Re-sort all elements into new grid
	}
	else
	{
		unitSize = size;
	}
}

// Do not update with tick, for objects that do not move in the scene
void ASpatialGrid::MakeStatic()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Update with tick, for objects that move in the scene
void ASpatialGrid::MakeDynamic()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Adds new element reference to grid, returning the grid coordinate
FVector ASpatialGrid::AddElement(AActor* elem)
{
	FVector cell = elem->GetActorLocation() / unitSize;
	grid[cell].AddElement(elem);
	return cell;
}

// Removes specified element from grid
void ASpatialGrid::RemoveElement(AActor* elem)
{
	std::map<FVector, Cell, FVectorCmp2>::iterator cell = grid.begin();
	bool removed = false;
	while (!removed && cell != grid.end())
	{
		removed = (cell++)->second.RemoveElement(elem);
	}
}

// Frees memory from cells with no elements
void ASpatialGrid::ClearPassive()
{
	std::map<FVector, Cell, FVectorCmp2>::iterator cell = grid.begin();
	while (cell != grid.end())
	{
		if (!cell->second.isActive())
		{
			grid.erase(cell);
		}
		++cell;
	}
}

// Called when the game starts or when spawned
void ASpatialGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

/****************************************
*  Cell Nested Class
*****************************************/

/*************************************
*  Basic Public Functions
**************************************/
ASpatialGrid::Cell::Cell() 
{
	
}

ASpatialGrid::Cell::~Cell()
{

}

// Returns list of all element pointers
std::list<AActor*> ASpatialGrid::Cell::GetElements()
{
	return elements;
}

// Returns number of entities in cell
int ASpatialGrid::Cell::GetSize()
{
	return elements.size();
}

// Returns whether cell currently has any elements
bool ASpatialGrid::Cell::isActive()
{
	return (elements.size() != 0);
}

// Adds new element reference to cell
void ASpatialGrid::Cell::AddElement(AActor* elem)
{
	elements.push_back(elem);
}

// Destroys all elements in cell
void ASpatialGrid::Cell::ClearCell()
{
	for (const auto elem : elements)
	{
		elem->ConditionalBeginDestroy();
	}
	elements.clear();
}

// Removes specified element from cell, returning whether element was found and removed or not
bool ASpatialGrid::Cell::RemoveElement(AActor* elem)
{
	std::list<AActor*>::iterator it = elements.begin();
	while (it != elements.end())
	{
		if (*it == elem)
		{
			elements.erase(it);
			return true;
		}
	}
	return false;
}

// Checks bounds and moves any elements outside of bounds to the proper cell
void ASpatialGrid::CleanCell(FVector cell)
{
	std::list<AActor*> elements = grid[cell].GetElements();
	FVector zero = cell * unitSize;

	std::list<AActor*>::iterator it = elements.begin();
	while (it != elements.end())
	{
		FVector loc = (*it)->GetActorLocation();
		// aabb bounds check
		if (loc.X < zero.X || loc.X >= zero.X + unitSize ||
			 loc.Y < zero.Y || loc.Y >= zero.Y + unitSize ||
			 loc.Z < zero.Z || loc.Z >= zero.Z + unitSize)
		{ // out of bounds, remove from cell and re-add to grid in proper cell
			grid[cell].RemoveElement(*it);
			AddElement(*it);
		}
		++it;
	}
}

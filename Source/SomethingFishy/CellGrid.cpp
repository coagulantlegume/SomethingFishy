// Fill out your copyright notice in the Description page of Project Settings.


#include "CellGrid.h"

#include "DrawDebugHelpers.h"

/* FVector less than comparison for map use */
bool FVectorCmp::operator()(const FVector& a, const FVector& b) const {
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
UCellGrid::UCellGrid()
{
	PrimaryComponentTick.bCanEverTick = false;
	// PrimaryComponentTick.TickInterval = 0.33f;
	unitSize = 1;
	activeCount = 0;
	passiveCount = 0;
	size = 0;
}

void UCellGrid::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	std::map<FVector, Cell, FVectorCmp>::iterator cell = grid.begin();
	passiveCount = 0;
	while (cell != grid.end())
	{
		if (cell->second.isActive())
		{
			CleanCell(cell->first);
			// DrawDebugBox(GetWorld(), (cell->first + 0.5) * unitSize, FVector(unitSize / 2, unitSize / 2, unitSize / 2), FColor::Green, false, .05, 0, 10);
		}
		else
		{
			++passiveCount;
			// DrawDebugBox(GetWorld(), (cell->first + 0.5) * unitSize, FVector(unitSize / 2, unitSize / 2, unitSize / 2), FColor::Red, false, .05, 0, 10);
		}
		++cell;
	}
	activeCount = size - passiveCount;
	if(grid.size() > 1.5 * size) ClearPassive();
}

int UCellGrid::GetUnitSize()
{
	return unitSize;
}

int UCellGrid::GetActiveCount()
{
	return activeCount;
}

int UCellGrid::GetPassiveCount()
{
	return passiveCount;
}

// returns list of all actors in same cell and neighboring cells
std::vector<AActor*> UCellGrid::GetNeighbors(const AActor* me)
{
	FVector loc = me->GetActorLocation() / unitSize;
	loc.X = floor(loc.X);
	loc.Y = floor(loc.Y);
	loc.Z = floor(loc.Z);

	std::vector<AActor*> neighbors;
	for (int x = loc.X - 1; x <= loc.X + 1; ++x)
	{
		for (int y = loc.Y - 1; y <= loc.Y + 1; ++y)
		{
			for (int z = loc.Z - 1; z <= loc.Z + 1; ++z)
			{
				if (grid.find(FVector(x, y, z)) != grid.end()) {
					std::list<AActor*> newNeighbors = grid[FVector(x, y, z)].GetElements();
					for (const auto& elem : newNeighbors)
					{
						neighbors.push_back(elem);
					}
				}
			}
		}
	}
	return neighbors;
}

// Set the unit size used for cells
void UCellGrid::setUnitSize(int uSize)
{
//	ClearPassive();
	if (GetActiveCount())
	{
		// TODO: Re-sort all elements into new grid
	}
	else
	{
		unitSize = uSize;
	}
}

// Do not update with tick, for objects that do not move in the scene
void UCellGrid::MakeStatic()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Update with tick, for objects that move in the scene
void UCellGrid::MakeDynamic()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Adds new element reference to grid, returning the grid coordinate
FVector UCellGrid::AddElement(AActor* elem)
{
	FVector cell = elem->GetActorLocation() / unitSize;

	// round down coordinates
	cell.X = floor(cell.X);
	cell.Y = floor(cell.Y);
	cell.Z = floor(cell.Z);

	grid[cell].AddElement(elem);
	++size;

	return cell;
}

// Removes specified element from grid
void UCellGrid::RemoveElement(AActor* elem)
{
	std::map<FVector, Cell, FVectorCmp>::iterator cell = grid.begin();
	bool removed = false;
	while (!removed && cell != grid.end())
	{
		removed = (cell++)->second.RemoveElement(elem);
	}

	if (removed)
	{
		--size;
	}
}

// Frees memory from cells with no elements
void UCellGrid::ClearPassive()
{
	std::map<FVector, Cell, FVectorCmp>::iterator cell = grid.begin();
	while (cell != grid.end())
	{
		if (!cell->second.isActive())
		{
			grid.erase(cell->first);
		}
		++cell;
	}
}

void UCellGrid::BeginPlay()
{
	Super::BeginPlay();
	
	//DrawDebugBox(GetWorld(), FVector(4000,4000,0), FVector(4000, 4000, 30), FColor::Green, true, -1, 0, 10);
}

/****************************************
*  Cell Nested Class
*****************************************/

/*************************************
*  Basic Public Functions
**************************************/
UCellGrid::Cell::Cell()
{

}

UCellGrid::Cell::~Cell()
{

}

// Returns list of all element pointers
std::list<AActor*> UCellGrid::Cell::GetElements()
{
	return elements;
}

// Returns number of entities in cell
int UCellGrid::Cell::GetSize()
{
	return elements.size();
}

// Returns whether cell currently has any elements
bool UCellGrid::Cell::isActive()
{
	return (elements.size() != 0);
}

// Adds new element reference to cell
void UCellGrid::Cell::AddElement(AActor* elem)
{
	elements.push_back(elem);
}

// Destroys all elements in cell
void UCellGrid::Cell::ClearCell()
{
	for (const auto elem : elements)
	{
		elem->ConditionalBeginDestroy();
	}
	elements.clear();
}

// Removes specified element from cell, returning whether element was found and removed or not
bool UCellGrid::Cell::RemoveElement(AActor* elem)
{
	std::list<AActor*>::iterator it = elements.begin();
	while (it != elements.end())
	{
		if (*it == elem)
		{
			elements.erase(it);
			return true;
		}
		++it;
	}
	return false;
}

// Checks bounds and moves any elements outside of bounds to the proper cell
void UCellGrid::CleanCell(FVector cell)
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
			--size;
			AddElement(*it);
		}
		++it;
	}
}

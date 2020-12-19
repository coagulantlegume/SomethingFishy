#include "Vector3Map.h"

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

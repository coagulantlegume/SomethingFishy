#pragma once

#include <Map>

/* FVector less than comparison for map use */
struct FVectorCmp {
	bool operator()(const FVector& a, const FVector& b) const;
};


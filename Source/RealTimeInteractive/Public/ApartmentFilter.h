#pragma once

#include "CoreMinimal.h"

struct FApartmentFilter
{
	int32 MinPrice = 0;
	TOptional<int32> MaxPrice;
	TOptional<int32> Rooms;
	TOptional<bool> HasGarage;
};
#pragma once

#include "CoreMinimal.h"

class REALTIMEINTERACTIVE_API FApartmentIdMapping
{
public:
	// "SM_Apartment001" -> "APT_001"
	static FString MeshNameToExternalId(const FString& MeshName);

	// "APT_001" -> "SM_Apartment001"
	static FString ExternalIdToMeshName(const FString& ExternalId);
};
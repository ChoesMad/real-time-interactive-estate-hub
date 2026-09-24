#include "ApartmentIdMapping.h"

namespace
{
	const FString MeshPrefix = TEXT("SM_Apartment");
	const FString ExternalIdPrefix = TEXT("APT_");
}

FString FApartmentIdMapping::MeshNameToExternalId(const FString& MeshName)
{
	if (!MeshName.StartsWith(MeshPrefix))
	{
		return FString();
	}

	const FString Number = MeshName.RightChop(MeshPrefix.Len());
	return ExternalIdPrefix + Number;
}

FString FApartmentIdMapping::ExternalIdToMeshName(const FString& ExternalId)
{
	if (!ExternalId.StartsWith(ExternalIdPrefix))
	{
		return FString();
	}

	const FString Number = ExternalId.RightChop(ExternalIdPrefix.Len());
	return MeshPrefix + Number;
}
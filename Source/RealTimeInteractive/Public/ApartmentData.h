#pragma once

#include "CoreMinimal.h"
#include "ApartmentData.generated.h"

USTRUCT(BlueprintType)
struct FApartmentData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString externalId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 price = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 rooms = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString status;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool hasGarage = false;
};
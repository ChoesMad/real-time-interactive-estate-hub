#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ApartmentData.h"
#include "ApartmentActor.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;

UCLASS()
class REALTIMEINTERACTIVE_API AApartmentActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	const FString& GetExternalId() const { return ExternalId; }
	const TOptional<FApartmentData>& GetMatchedApartment() const { return MatchedApartment; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Apartment")
	FString ExternalId;

private:
	UFUNCTION()
	void OnApartmentsLoaded();

	UFUNCTION()
	void OnFilterChanged();

	FString ResolveExternalIdFromMeshName() const;

	void RefreshStatusColor();
	void ApplyStatusColor(const FString& Status);
	void ResetStatusColor();

	UPROPERTY()
	UMaterialInstanceDynamic* GlassStatusMID;

	UPROPERTY()
	UMaterialInterface* OriginalGlassMaterial;

	TOptional<FApartmentData> MatchedApartment;
};
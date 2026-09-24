#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "ApartmentData.h"
#include "ApartmentFilter.h"
#include "EstateApiSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnApartmentsLoaded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFilterChanged);

UCLASS()
class REALTIMEINTERACTIVE_API UEstateApiSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void FetchApartments();

	const TArray<FApartmentData>& GetApartments() const { return Apartments; }

	void SetFilter(const FApartmentFilter& NewFilter);
	const FApartmentFilter& GetCurrentFilter() const { return CurrentFilter; }
	bool MatchesFilter(const FApartmentData& Apartment) const;

	void ReserveApartment(const FString& ExternalId);
	void ReleaseReservation();
	void PurchaseApartment(const FString& ExternalId);

	const TOptional<FString>& GetCurrentReservationExternalId() const { return CurrentReservationExternalId; }
	int32 GetAvailableReservations() const { return AvailableReservations; }
	int32 GetAmount() const { return Amount; }

	static constexpr int32 StartingAmount = 700000;

	UPROPERTY(BlueprintAssignable)
	FOnApartmentsLoaded OnApartmentsLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnFilterChanged OnFilterChanged;

private:
	void HandleFetchApartmentsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleReserveResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString ExternalId);
	void HandleReleaseResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString ExternalId);
	void HandlePurchaseResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString ExternalId);
	void HandleDebugSetFilterCommand(const TArray<FString>& Args);

	TArray<FApartmentData> Apartments;
	FApartmentFilter CurrentFilter;

	TOptional<FString> CurrentReservationExternalId;
	int32 AvailableReservations = 1;

	int32 Amount = StartingAmount;

	IConsoleCommand* DebugSetFilterConsoleCommand = nullptr;
};
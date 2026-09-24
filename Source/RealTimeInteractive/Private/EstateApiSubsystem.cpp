#include "EstateApiSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"
#include "HAL/IConsoleManager.h"

void UEstateApiSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FetchApartments();

	DebugSetFilterConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Estate.SetFilter"),
		TEXT("Estate.SetFilter <minPrice> <maxPrice|-1> <rooms|-1> <hasGarage 0/1/-1>"),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UEstateApiSubsystem::HandleDebugSetFilterCommand),
		ECVF_Default);
}

void UEstateApiSubsystem::Deinitialize()
{
	if (DebugSetFilterConsoleCommand)
	{
		IConsoleManager::Get().UnregisterConsoleObject(DebugSetFilterConsoleCommand);
		DebugSetFilterConsoleCommand = nullptr;
	}

	Super::Deinitialize();
}

void UEstateApiSubsystem::FetchApartments()
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://localhost:8080/api/v1/apartments"));
	Request->SetVerb(TEXT("GET"));
	Request->OnProcessRequestComplete().BindUObject(this, &UEstateApiSubsystem::HandleFetchApartmentsResponse);
	Request->ProcessRequest();
}

void UEstateApiSubsystem::HandleFetchApartmentsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UEstateApiSubsystem: request failed"));
		return;
	}

	Apartments.Empty();

	const bool bParsed = FJsonObjectConverter::JsonArrayStringToUStruct<FApartmentData>(
		Response->GetContentAsString(), &Apartments, 0, 0, false);

	if (!bParsed)
	{
		UE_LOG(LogTemp, Error, TEXT("UEstateApiSubsystem: failed to parse JSON response"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("UEstateApiSubsystem: parsed %d apartments"), Apartments.Num());

	OnApartmentsLoaded.Broadcast();
}

void UEstateApiSubsystem::SetFilter(const FApartmentFilter& NewFilter)
{
	CurrentFilter = NewFilter;
	OnFilterChanged.Broadcast();
}

bool UEstateApiSubsystem::MatchesFilter(const FApartmentData& Apartment) const
{
	if (Apartment.price < CurrentFilter.MinPrice)
	{
		return false;
	}

	if (CurrentFilter.MaxPrice.IsSet() && Apartment.price > CurrentFilter.MaxPrice.GetValue())
	{
		return false;
	}

	if (CurrentFilter.Rooms.IsSet() && Apartment.rooms != CurrentFilter.Rooms.GetValue())
	{
		return false;
	}

	if (CurrentFilter.HasGarage.IsSet() && Apartment.hasGarage != CurrentFilter.HasGarage.GetValue())
	{
		return false;
	}

	return true;
}

void UEstateApiSubsystem::ReserveApartment(const FString& ExternalId)
{
	if (CurrentReservationExternalId.IsSet())
	{
		UE_LOG(LogTemp, Warning, TEXT("UEstateApiSubsystem: player already has an active reservation (%s)"), *CurrentReservationExternalId.GetValue());
		return;
	}

	const FApartmentData* TargetApartment = nullptr;
	for (const FApartmentData& Apartment : Apartments)
	{
		if (Apartment.externalId == ExternalId)
		{
			TargetApartment = &Apartment;
			break;
		}
	}

	if (!TargetApartment)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEstateApiSubsystem: cannot reserve unknown apartment %s"), *ExternalId);
		return;
	}

	if (Amount < TargetApartment->price)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEstateApiSubsystem: insufficient funds to reserve %s (have %d, need %d)"),
			*ExternalId, Amount, TargetApartment->price);
		return;
	}

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(FString::Printf(TEXT("http://localhost:8080/api/v1/apartments/%s/reserve"), *ExternalId));
	Request->SetVerb(TEXT("PATCH"));
	Request->OnProcessRequestComplete().BindUObject(this, &UEstateApiSubsystem::HandleReserveResponse, ExternalId);
	Request->ProcessRequest();
}

void UEstateApiSubsystem::HandleReserveResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString ExternalId)
{
	if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEstateApiSubsystem: reserve failed for %s (code %d)"),
			*ExternalId, Response.IsValid() ? Response->GetResponseCode() : -1);
		return;
	}

	for (FApartmentData& Apartment : Apartments)
	{
		if (Apartment.externalId == ExternalId)
		{
			Apartment.status = TEXT("RESERVED");
			break;
		}
	}

	CurrentReservationExternalId = ExternalId;
	AvailableReservations--;

	UE_LOG(LogTemp, Log, TEXT("UEstateApiSubsystem: reserved %s (available reservations: %d)"), *ExternalId, AvailableReservations);

	OnApartmentsLoaded.Broadcast();
}

void UEstateApiSubsystem::ReleaseReservation()
{
	if (!CurrentReservationExternalId.IsSet())
	{
		return;
	}

	const FString ExternalId = CurrentReservationExternalId.GetValue();

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(FString::Printf(TEXT("http://localhost:8080/api/v1/apartments/%s/release"), *ExternalId));
	Request->SetVerb(TEXT("PATCH"));
	Request->OnProcessRequestComplete().BindUObject(this, &UEstateApiSubsystem::HandleReleaseResponse, ExternalId);
	Request->ProcessRequest();
}

void UEstateApiSubsystem::HandleReleaseResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString ExternalId)
{
	if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEstateApiSubsystem: release failed for %s (code %d)"),
			*ExternalId, Response.IsValid() ? Response->GetResponseCode() : -1);
		return;
	}

	for (FApartmentData& Apartment : Apartments)
	{
		if (Apartment.externalId == ExternalId)
		{
			Apartment.status = TEXT("AVAILABLE");
			break;
		}
	}

	CurrentReservationExternalId.Reset();
	AvailableReservations++;

	UE_LOG(LogTemp, Log, TEXT("UEstateApiSubsystem: released %s (available reservations: %d)"), *ExternalId, AvailableReservations);

	OnApartmentsLoaded.Broadcast();
}

void UEstateApiSubsystem::PurchaseApartment(const FString& ExternalId)
{
	if (!CurrentReservationExternalId.IsSet() || CurrentReservationExternalId.GetValue() != ExternalId)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEstateApiSubsystem: cannot purchase %s — not your active reservation"), *ExternalId);
		return;
	}

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(FString::Printf(TEXT("http://localhost:8080/api/v1/apartments/%s/purchase"), *ExternalId));
	Request->SetVerb(TEXT("PATCH"));
	Request->OnProcessRequestComplete().BindUObject(this, &UEstateApiSubsystem::HandlePurchaseResponse, ExternalId);
	Request->ProcessRequest();
}

void UEstateApiSubsystem::HandlePurchaseResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString ExternalId)
{
	if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEstateApiSubsystem: purchase failed for %s (code %d)"),
			*ExternalId, Response.IsValid() ? Response->GetResponseCode() : -1);
		return;
	}

	for (FApartmentData& Apartment : Apartments)
	{
		if (Apartment.externalId == ExternalId)
		{
			Apartment.status = TEXT("SOLD");
			Amount -= Apartment.price;
			break;
		}
	}

	CurrentReservationExternalId.Reset();
	AvailableReservations++;

	UE_LOG(LogTemp, Log, TEXT("UEstateApiSubsystem: purchased %s (remaining amount: %d)"), *ExternalId, Amount);

	OnApartmentsLoaded.Broadcast();
}

void UEstateApiSubsystem::HandleDebugSetFilterCommand(const TArray<FString>& Args)
{
	if (Args.Num() != 4)
	{
		UE_LOG(LogTemp, Warning, TEXT("Estate.SetFilter: wymagane 4 argumenty: minPrice maxPrice(-1=Any) rooms(-1=Any) hasGarage(-1=Any,0=Nie,1=Tak)"));
		return;
	}

	FApartmentFilter NewFilter;
	NewFilter.MinPrice = FCString::Atoi(*Args[0]);

	const int32 MaxPriceArg = FCString::Atoi(*Args[1]);
	if (MaxPriceArg >= 0)
	{
		NewFilter.MaxPrice = MaxPriceArg;
	}

	const int32 RoomsArg = FCString::Atoi(*Args[2]);
	if (RoomsArg >= 0)
	{
		NewFilter.Rooms = RoomsArg;
	}

	const int32 GarageArg = FCString::Atoi(*Args[3]);
	if (GarageArg == 0 || GarageArg == 1)
	{
		NewFilter.HasGarage = (GarageArg == 1);
	}

	SetFilter(NewFilter);
}
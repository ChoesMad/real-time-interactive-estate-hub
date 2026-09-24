#include "ApartmentDetailsWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "EstateApiSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UApartmentDetailsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ActionButton)
	{
		ActionButton->OnClicked.AddDynamic(this, &UApartmentDetailsWidget::HandleActionButtonClicked);
	}

	if (PurchaseButton)
	{
		PurchaseButton->OnClicked.AddDynamic(this, &UApartmentDetailsWidget::HandlePurchaseButtonClicked);
	}

	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UEstateApiSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<UEstateApiSubsystem>() : nullptr;
	if (Subsystem)
	{
		Subsystem->OnApartmentsLoaded.AddDynamic(this, &UApartmentDetailsWidget::OnApartmentsChanged);
	}

	ClearDetails();
}

void UApartmentDetailsWidget::ShowApartmentDetails(const FString& InExternalId)
{
	CurrentlyDisplayedExternalId = InExternalId;
	RefreshDisplay();
}

void UApartmentDetailsWidget::ClearDetails()
{
	CurrentlyDisplayedExternalId.Reset();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UApartmentDetailsWidget::OnApartmentsChanged()
{
	if (CurrentlyDisplayedExternalId.IsSet())
	{
		RefreshDisplay();
	}
}

void UApartmentDetailsWidget::RefreshDisplay()
{
	if (!CurrentlyDisplayedExternalId.IsSet())
	{
		return;
	}

	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UEstateApiSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<UEstateApiSubsystem>() : nullptr;
	if (!Subsystem)
	{
		return;
	}

	const FApartmentData* Found = nullptr;
	for (const FApartmentData& Apartment : Subsystem->GetApartments())
	{
		if (Apartment.externalId == CurrentlyDisplayedExternalId.GetValue())
		{
			Found = &Apartment;
			break;
		}
	}

	if (!Found)
	{
		ClearDetails();
		return;
	}

	SetVisibility(ESlateVisibility::Visible);

	if (ExternalIdText) { ExternalIdText->SetText(FText::FromString(Found->externalId)); }
	if (PriceText) { PriceText->SetText(FText::FromString(FString::Printf(TEXT("PRICE: %s PLN"), *FText::AsNumber(Found->price).ToString()))); }
	if (RoomsText) { RoomsText->SetText(FText::FromString(FString::Printf(TEXT("ROOMS: %d"), Found->rooms))); }
	if (GarageText) { GarageText->SetText(FText::FromString(FString::Printf(TEXT("HAS GARAGE: %s"), Found->hasGarage ? TEXT("YES") : TEXT("NO")))); }
	if (StatusText) { StatusText->SetText(FText::FromString(FString::Printf(TEXT("STATUS: %s"), *Found->status))); }
	if (ReservationCountText) { ReservationCountText->SetText(FText::FromString(FString::Printf(TEXT("REMAINING RESERVATION: %d"), Subsystem->GetAvailableReservations()))); }

	const bool bIsMyReservation = Subsystem->GetCurrentReservationExternalId().IsSet()
		&& Subsystem->GetCurrentReservationExternalId().GetValue() == Found->externalId;
	const bool bPlayerHasAnyReservation = Subsystem->GetCurrentReservationExternalId().IsSet();

	if (ActionButton)
	{
		if (Found->status == TEXT("SOLD"))
		{
			ActionButton->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (Found->status == TEXT("RESERVED") && bIsMyReservation)
		{
			ActionButton->SetVisibility(ESlateVisibility::Visible);
			if (ActionButtonLabel) { ActionButtonLabel->SetText(FText::FromString(TEXT("CANCEL RESERVATION"))); }
		}
		else if (Found->status == TEXT("AVAILABLE") && !bPlayerHasAnyReservation && Subsystem->GetAmount() >= Found->price)
		{
			ActionButton->SetVisibility(ESlateVisibility::Visible);
			if (ActionButtonLabel) { ActionButtonLabel->SetText(FText::FromString(TEXT("BOOK NOW"))); }
		}
		else
		{
			ActionButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (PurchaseButton)
	{
		PurchaseButton->SetVisibility(
			(Found->status == TEXT("RESERVED") && bIsMyReservation) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UApartmentDetailsWidget::HandleActionButtonClicked()
{
	if (!CurrentlyDisplayedExternalId.IsSet())
	{
		return;
	}

	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UEstateApiSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<UEstateApiSubsystem>() : nullptr;
	if (!Subsystem)
	{
		return;
	}

	const bool bIsMyReservation = Subsystem->GetCurrentReservationExternalId().IsSet()
		&& Subsystem->GetCurrentReservationExternalId().GetValue() == CurrentlyDisplayedExternalId.GetValue();

	if (bIsMyReservation)
	{
		Subsystem->ReleaseReservation();
	}
	else
	{
		Subsystem->ReserveApartment(CurrentlyDisplayedExternalId.GetValue());
	}
}

void UApartmentDetailsWidget::HandlePurchaseButtonClicked()
{
	if (!CurrentlyDisplayedExternalId.IsSet())
	{
		return;
	}

	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UEstateApiSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<UEstateApiSubsystem>() : nullptr;
	if (!Subsystem)
	{
		return;
	}

	Subsystem->PurchaseApartment(CurrentlyDisplayedExternalId.GetValue());
}
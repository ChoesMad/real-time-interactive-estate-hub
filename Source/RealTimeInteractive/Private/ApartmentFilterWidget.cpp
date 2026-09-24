#include "ApartmentFilterWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "EstateApiSubsystem.h"
#include "ApartmentFilter.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UApartmentFilterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MinPriceSlider)
	{
		MinPriceSlider->SetMinValue(0.f);
		MinPriceSlider->SetMaxValue(static_cast<float>(MaxPriceCap));
		MinPriceSlider->SetValue(0.f);
		MinPriceSlider->OnValueChanged.AddDynamic(this, &UApartmentFilterWidget::HandleMinPriceChanged);
	}

	if (MaxPriceSlider)
	{
		MaxPriceSlider->SetMinValue(0.f);
		MaxPriceSlider->SetMaxValue(static_cast<float>(MaxPriceCap));
		MaxPriceSlider->SetValue(static_cast<float>(MaxPriceCap));
		MaxPriceSlider->OnValueChanged.AddDynamic(this, &UApartmentFilterWidget::HandleMaxPriceChanged);
	}

	if (RoomsAnyButton) { RoomsAnyButton->OnClicked.AddDynamic(this, &UApartmentFilterWidget::HandleRoomsAnyClicked); }
	if (Rooms1Button) { Rooms1Button->OnClicked.AddDynamic(this, &UApartmentFilterWidget::HandleRooms1Clicked); }
	if (Rooms2Button) { Rooms2Button->OnClicked.AddDynamic(this, &UApartmentFilterWidget::HandleRooms2Clicked); }
	if (Rooms3Button) { Rooms3Button->OnClicked.AddDynamic(this, &UApartmentFilterWidget::HandleRooms3Clicked); }

	if (GarageAnyButton) { GarageAnyButton->OnClicked.AddDynamic(this, &UApartmentFilterWidget::HandleGarageAnyClicked); }
	if (GarageYesButton) { GarageYesButton->OnClicked.AddDynamic(this, &UApartmentFilterWidget::HandleGarageYesClicked); }
	if (GarageNoButton) { GarageNoButton->OnClicked.AddDynamic(this, &UApartmentFilterWidget::HandleGarageNoClicked); }

	if (AcceptButton) { AcceptButton->OnClicked.AddDynamic(this, &UApartmentFilterWidget::HandleAcceptClicked); }
	if (CloseButton) { CloseButton->OnClicked.AddDynamic(this, &UApartmentFilterWidget::HandleCloseClicked); }

	WorkingMaxPrice.Reset();
	AppliedMaxPrice.Reset();

	RefreshPriceLabels();
	RefreshRoomsButtonHighlight();
	RefreshGarageButtonHighlight();
}

void UApartmentFilterWidget::SetPanelVisible(bool bVisible)
{
	if (FilterPanelRoot)
	{
		FilterPanelRoot->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UApartmentFilterWidget::ResetWorkingStateToApplied()
{
	WorkingMinPrice = AppliedMinPrice;
	WorkingMaxPrice = AppliedMaxPrice;
	WorkingRooms = AppliedRooms;
	WorkingHasGarage = AppliedHasGarage;

	if (MinPriceSlider)
	{
		MinPriceSlider->SetValue(static_cast<float>(WorkingMinPrice));
	}

	if (MaxPriceSlider)
	{
		MaxPriceSlider->SetValue(WorkingMaxPrice.IsSet() ? static_cast<float>(WorkingMaxPrice.GetValue()) : static_cast<float>(MaxPriceCap));
	}

	RefreshPriceLabels();
	RefreshRoomsButtonHighlight();
	RefreshGarageButtonHighlight();
}

void UApartmentFilterWidget::HandleMinPriceChanged(float NewValue)
{
	int32 NewMin = FMath::RoundToInt(NewValue);

	const int32 CurrentMaxAsInt = MaxPriceSlider ? FMath::RoundToInt(MaxPriceSlider->GetValue()) : MaxPriceCap;
	if (NewMin > CurrentMaxAsInt)
	{
		NewMin = CurrentMaxAsInt;
		if (MinPriceSlider) { MinPriceSlider->SetValue(static_cast<float>(NewMin)); }
	}

	WorkingMinPrice = NewMin;
	RefreshPriceLabels();
}

void UApartmentFilterWidget::HandleMaxPriceChanged(float NewValue)
{
	int32 NewMax = FMath::RoundToInt(NewValue);

	const int32 CurrentMinAsInt = MinPriceSlider ? FMath::RoundToInt(MinPriceSlider->GetValue()) : 0;
	if (NewMax < CurrentMinAsInt)
	{
		NewMax = CurrentMinAsInt;
		if (MaxPriceSlider) { MaxPriceSlider->SetValue(static_cast<float>(NewMax)); }
	}

	WorkingMaxPrice = (NewMax >= MaxPriceCap) ? TOptional<int32>() : TOptional<int32>(NewMax);
	RefreshPriceLabels();
}

void UApartmentFilterWidget::RefreshPriceLabels()
{
	if (MinPriceValueText)
	{
		MinPriceValueText->SetText(FText::AsNumber(WorkingMinPrice));
	}

	const FString MaxPriceString = WorkingMaxPrice.IsSet() ? FString::FromInt(WorkingMaxPrice.GetValue()) : TEXT("unlimited");

	if (MaxPriceValueText)
	{
		MaxPriceValueText->SetText(FText::FromString(MaxPriceString));
	}

	if (PriceRangeTitleText)
	{
		PriceRangeTitleText->SetText(FText::FromString(FString::Printf(TEXT("CENA : %d - %s"), WorkingMinPrice, *MaxPriceString)));
	}
}

void UApartmentFilterWidget::HandleRoomsAnyClicked() { SetWorkingRooms(TOptional<int32>()); }
void UApartmentFilterWidget::HandleRooms1Clicked() { SetWorkingRooms(1); }
void UApartmentFilterWidget::HandleRooms2Clicked() { SetWorkingRooms(2); }
void UApartmentFilterWidget::HandleRooms3Clicked() { SetWorkingRooms(3); }

void UApartmentFilterWidget::SetWorkingRooms(TOptional<int32> Rooms)
{
	WorkingRooms = Rooms;
	RefreshRoomsButtonHighlight();
}

void UApartmentFilterWidget::HandleGarageAnyClicked() { SetWorkingGarage(TOptional<bool>()); }
void UApartmentFilterWidget::HandleGarageYesClicked() { SetWorkingGarage(true); }
void UApartmentFilterWidget::HandleGarageNoClicked() { SetWorkingGarage(false); }

void UApartmentFilterWidget::SetWorkingGarage(TOptional<bool> HasGarage)
{
	WorkingHasGarage = HasGarage;
	RefreshGarageButtonHighlight();
}

void UApartmentFilterWidget::RefreshRoomsButtonHighlight()
{
	const FLinearColor Selected(1.f, 1.f, 1.f, 1.f);
	const FLinearColor Unselected(0.5f, 0.5f, 0.5f, 1.f);

	if (RoomsAnyButton) { RoomsAnyButton->SetBackgroundColor(!WorkingRooms.IsSet() ? Selected : Unselected); }
	if (Rooms1Button) { Rooms1Button->SetBackgroundColor((WorkingRooms.IsSet() && WorkingRooms.GetValue() == 1) ? Selected : Unselected); }
	if (Rooms2Button) { Rooms2Button->SetBackgroundColor((WorkingRooms.IsSet() && WorkingRooms.GetValue() == 2) ? Selected : Unselected); }
	if (Rooms3Button) { Rooms3Button->SetBackgroundColor((WorkingRooms.IsSet() && WorkingRooms.GetValue() == 3) ? Selected : Unselected); }
}

void UApartmentFilterWidget::RefreshGarageButtonHighlight()
{
	const FLinearColor Selected(1.f, 1.f, 1.f, 1.f);
	const FLinearColor Unselected(0.5f, 0.5f, 0.5f, 1.f);

	if (GarageAnyButton) { GarageAnyButton->SetBackgroundColor(!WorkingHasGarage.IsSet() ? Selected : Unselected); }
	if (GarageYesButton) { GarageYesButton->SetBackgroundColor((WorkingHasGarage.IsSet() && WorkingHasGarage.GetValue()) ? Selected : Unselected); }
	if (GarageNoButton) { GarageNoButton->SetBackgroundColor((WorkingHasGarage.IsSet() && !WorkingHasGarage.GetValue()) ? Selected : Unselected); }
}

void UApartmentFilterWidget::HandleAcceptClicked()
{
	AppliedMinPrice = WorkingMinPrice;
	AppliedMaxPrice = WorkingMaxPrice;
	AppliedRooms = WorkingRooms;
	AppliedHasGarage = WorkingHasGarage;

	ApplyFilterToSubsystem();

	OnPanelCloseRequested.Broadcast();
}

void UApartmentFilterWidget::HandleCloseClicked()
{
	OnPanelCloseRequested.Broadcast();
}

void UApartmentFilterWidget::ApplyFilterToSubsystem()
{
	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UEstateApiSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<UEstateApiSubsystem>() : nullptr;

	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("UApartmentFilterWidget: UEstateApiSubsystem not found"));
		return;
	}

	FApartmentFilter NewFilter;
	NewFilter.MinPrice = AppliedMinPrice;
	NewFilter.MaxPrice = AppliedMaxPrice;
	NewFilter.Rooms = AppliedRooms;
	NewFilter.HasGarage = AppliedHasGarage;

	Subsystem->SetFilter(NewFilter);
}
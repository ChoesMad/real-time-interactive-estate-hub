#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ApartmentFilterWidget.generated.h"

class USlider;
class UTextBlock;
class UButton;
class UWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPanelCloseRequested);

UCLASS()
class REALTIMEINTERACTIVE_API UApartmentFilterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetPanelVisible(bool bVisible);
	void ResetWorkingStateToApplied();

	UPROPERTY(BlueprintAssignable)
	FOnPanelCloseRequested OnPanelCloseRequested;

protected:
	UPROPERTY(meta = (BindWidget))
	UWidget* FilterPanelRoot;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PriceRangeTitleText;

	UPROPERTY(meta = (BindWidget))
	USlider* MinPriceSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* MaxPriceSlider;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MinPriceValueText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxPriceValueText;

	UPROPERTY(meta = (BindWidget))
	UButton* RoomsAnyButton;

	UPROPERTY(meta = (BindWidget))
	UButton* Rooms1Button;

	UPROPERTY(meta = (BindWidget))
	UButton* Rooms2Button;

	UPROPERTY(meta = (BindWidget))
	UButton* Rooms3Button;

	UPROPERTY(meta = (BindWidget))
	UButton* GarageAnyButton;

	UPROPERTY(meta = (BindWidget))
	UButton* GarageYesButton;

	UPROPERTY(meta = (BindWidget))
	UButton* GarageNoButton;

	UPROPERTY(meta = (BindWidget))
	UButton* AcceptButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;

private:
	UFUNCTION()
	void HandleMinPriceChanged(float NewValue);

	UFUNCTION()
	void HandleMaxPriceChanged(float NewValue);

	UFUNCTION()
	void HandleRoomsAnyClicked();

	UFUNCTION()
	void HandleRooms1Clicked();

	UFUNCTION()
	void HandleRooms2Clicked();

	UFUNCTION()
	void HandleRooms3Clicked();

	UFUNCTION()
	void HandleGarageAnyClicked();

	UFUNCTION()
	void HandleGarageYesClicked();

	UFUNCTION()
	void HandleGarageNoClicked();

	UFUNCTION()
	void HandleAcceptClicked();

	UFUNCTION()
	void HandleCloseClicked();

	void SetWorkingRooms(TOptional<int32> Rooms);
	void SetWorkingGarage(TOptional<bool> HasGarage);
	void ApplyFilterToSubsystem();
	void RefreshPriceLabels();
	void RefreshRoomsButtonHighlight();
	void RefreshGarageButtonHighlight();

	static constexpr int32 MaxPriceCap = 1000000;

	int32 WorkingMinPrice = 0;
	TOptional<int32> WorkingMaxPrice;
	TOptional<int32> WorkingRooms;
	TOptional<bool> WorkingHasGarage;

	int32 AppliedMinPrice = 0;
	TOptional<int32> AppliedMaxPrice;
	TOptional<int32> AppliedRooms;
	TOptional<bool> AppliedHasGarage;
};
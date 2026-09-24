#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ApartmentDetailsWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class REALTIMEINTERACTIVE_API UApartmentDetailsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void ShowApartmentDetails(const FString& InExternalId);
	void ClearDetails();

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExternalIdText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PriceText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoomsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GarageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StatusText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ReservationCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmountText;

	UPROPERTY(meta = (BindWidget))
	UButton* ActionButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ActionButtonLabel;

	UPROPERTY(meta = (BindWidget))
	UButton* PurchaseButton;

private:
	UFUNCTION()
	void OnApartmentsChanged();

	UFUNCTION()
	void HandleActionButtonClicked();

	UFUNCTION()
	void HandlePurchaseButtonClicked();

	void RefreshDisplay();

	TOptional<FString> CurrentlyDisplayedExternalId;
};
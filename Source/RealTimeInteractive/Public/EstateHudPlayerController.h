#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EstateHudPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UApartmentFilterWidget;
class UApartmentDetailsWidget;
struct FInputActionValue;

UCLASS()
class REALTIMEINTERACTIVE_API AEstateHudPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Estate Hud")
	UInputMappingContext* EstateHudMappingContext;

	UPROPERTY(EditAnywhere, Category = "Estate Hud")
	UInputAction* ToggleFilterPanelAction;

	UPROPERTY(EditAnywhere, Category = "Estate Hud")
	UInputAction* SelectApartmentAction;

	UPROPERTY(EditAnywhere, Category = "Estate Hud")
	TSubclassOf<UApartmentFilterWidget> FilterWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Estate Hud")
	TSubclassOf<UApartmentDetailsWidget> DetailsWidgetClass;

private:
	void HandleToggleFilterPanel(const FInputActionValue& Value);
	void HandleSelectApartment(const FInputActionValue& Value);

	void ShowFilterPanel();
	void HideFilterPanel();

	UFUNCTION()
	void HandlePanelCloseRequested();

	UPROPERTY()
	UApartmentFilterWidget* FilterWidgetInstance;

	UPROPERTY()
	UApartmentDetailsWidget* DetailsWidgetInstance;

	bool bFilterPanelVisible = false;
};
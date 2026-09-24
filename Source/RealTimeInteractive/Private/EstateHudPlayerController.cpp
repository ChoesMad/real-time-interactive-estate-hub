#include "EstateHudPlayerController.h"
#include "ApartmentFilterWidget.h"
#include "ApartmentDetailsWidget.h"
#include "ApartmentActor.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Engine/HitResult.h"
#include "EstateApiSubsystem.h"

void AEstateHudPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (EstateHudMappingContext)
			{
				Subsystem->AddMappingContext(EstateHudMappingContext, 0);
			}
		}
	}

	if (FilterWidgetClass)
	{
		FilterWidgetInstance = CreateWidget<UApartmentFilterWidget>(this, FilterWidgetClass);
		if (FilterWidgetInstance)
		{
			FilterWidgetInstance->AddToViewport();
			FilterWidgetInstance->SetPanelVisible(false);
			FilterWidgetInstance->OnPanelCloseRequested.AddDynamic(this, &AEstateHudPlayerController::HandlePanelCloseRequested);
		}
	}

	if (DetailsWidgetClass)
	{
		DetailsWidgetInstance = CreateWidget<UApartmentDetailsWidget>(this, DetailsWidgetClass);
		if (DetailsWidgetInstance)
		{
			DetailsWidgetInstance->AddToViewport();
			DetailsWidgetInstance->ClearDetails();
		}
	}

	bFilterPanelVisible = false;
}

void AEstateHudPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (ToggleFilterPanelAction)
		{
			EnhancedInputComponent->BindAction(ToggleFilterPanelAction, ETriggerEvent::Started, this, &AEstateHudPlayerController::HandleToggleFilterPanel);
		}

		if (SelectApartmentAction)
		{
			EnhancedInputComponent->BindAction(SelectApartmentAction, ETriggerEvent::Started, this, &AEstateHudPlayerController::HandleSelectApartment);
		}
	}
}

void AEstateHudPlayerController::HandleToggleFilterPanel(const FInputActionValue& Value)
{
	if (bFilterPanelVisible)
	{
		HideFilterPanel();
	}
	else
	{
		ShowFilterPanel();
	}
}

void AEstateHudPlayerController::HandleSelectApartment(const FInputActionValue& Value)
{
	if (!bFilterPanelVisible)
	{
		return;
	}

	FHitResult HitResult;
	if (!GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		return;
	}

	AApartmentActor* ClickedApartment = Cast<AApartmentActor>(HitResult.GetActor());
	if (!ClickedApartment || !ClickedApartment->GetMatchedApartment().IsSet())
	{
		return;
	}

	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UEstateApiSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<UEstateApiSubsystem>() : nullptr;

	if (!Subsystem || !Subsystem->MatchesFilter(ClickedApartment->GetMatchedApartment().GetValue()))
	{
		return;
	}

	if (DetailsWidgetInstance)
	{
		DetailsWidgetInstance->ShowApartmentDetails(ClickedApartment->GetExternalId());
	}
}

void AEstateHudPlayerController::ShowFilterPanel()
{
	if (!FilterWidgetInstance)
	{
		return;
	}

	FilterWidgetInstance->ResetWorkingStateToApplied();
	FilterWidgetInstance->SetPanelVisible(true);

	bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(FilterWidgetInstance->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	bFilterPanelVisible = true;
}

void AEstateHudPlayerController::HideFilterPanel()
{
	if (!FilterWidgetInstance)
	{
		return;
	}

	FilterWidgetInstance->SetPanelVisible(false);

	if (DetailsWidgetInstance)
	{
		DetailsWidgetInstance->ClearDetails();
	}

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	bFilterPanelVisible = false;
}

void AEstateHudPlayerController::HandlePanelCloseRequested()
{
	HideFilterPanel();
}
#include "ApartmentActor.h"
#include "ApartmentIdMapping.h"
#include "EstateApiSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

void AApartmentActor::BeginPlay()
{
	Super::BeginPlay();

	if (UStaticMeshComponent* MeshComponent = GetStaticMeshComponent())
	{
		OriginalGlassMaterial = MeshComponent->GetMaterial(1);
	}

	ExternalId = ResolveExternalIdFromMeshName();

	if (ExternalId.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("AApartmentActor (%s): could not resolve ExternalId from mesh name"), *GetName());
		return;
	}

	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UEstateApiSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<UEstateApiSubsystem>() : nullptr;

	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("AApartmentActor (%s): UEstateApiSubsystem not found"), *GetName());
		return;
	}

	Subsystem->OnFilterChanged.AddDynamic(this, &AApartmentActor::OnFilterChanged);

	if (Subsystem->GetApartments().Num() > 0)
	{
		OnApartmentsLoaded();
	}
	else
	{
		Subsystem->OnApartmentsLoaded.AddDynamic(this, &AApartmentActor::OnApartmentsLoaded);
	}
}

void AApartmentActor::OnApartmentsLoaded()
{
	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UEstateApiSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<UEstateApiSubsystem>() : nullptr;

	if (!Subsystem)
	{
		return;
	}

	MatchedApartment.Reset();

	for (const FApartmentData& Apartment : Subsystem->GetApartments())
	{
		if (Apartment.externalId == ExternalId)
		{
			MatchedApartment = Apartment;
			break;
		}
	}

	if (!MatchedApartment.IsSet())
	{
		UE_LOG(LogTemp, Warning, TEXT("AApartmentActor (%s): no matching apartment data for ExternalId=%s"), *GetName(), *ExternalId);
		return;
	}

	RefreshStatusColor();
}

void AApartmentActor::OnFilterChanged()
{
	RefreshStatusColor();
}

void AApartmentActor::RefreshStatusColor()
{
	if (!MatchedApartment.IsSet())
	{
		return;
	}

	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UEstateApiSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<UEstateApiSubsystem>() : nullptr;

	if (!Subsystem)
	{
		return;
	}

	if (Subsystem->MatchesFilter(MatchedApartment.GetValue()))
	{
		ApplyStatusColor(MatchedApartment.GetValue().status);
	}
	else
	{
		ResetStatusColor();
	}
}

void AApartmentActor::ApplyStatusColor(const FString& Status)
{
	UStaticMeshComponent* MeshComponent = GetStaticMeshComponent();
	if (!MeshComponent)
	{
		return;
	}

	if (!GlassStatusMID)
	{
		GlassStatusMID = MeshComponent->CreateDynamicMaterialInstance(1);
	}

	if (!GlassStatusMID)
	{
		UE_LOG(LogTemp, Warning, TEXT("AApartmentActor (%s): failed to create dynamic material instance on slot 1"), *GetName());
		return;
	}

	FLinearColor Color = FLinearColor::White;

	if (Status == TEXT("AVAILABLE"))
	{
		Color = FLinearColor::Green;
	}
	else if (Status == TEXT("RESERVED"))
	{
		Color = FLinearColor::Yellow;
	}
	else if (Status == TEXT("SOLD"))
	{
		Color = FLinearColor::Red;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AApartmentActor (%s): unknown status '%s'"), *GetName(), *Status);
		return;
	}

	GlassStatusMID->SetVectorParameterValue(TEXT("DiffuseColor"), Color);
}

void AApartmentActor::ResetStatusColor()
{
	UStaticMeshComponent* MeshComponent = GetStaticMeshComponent();
	if (!MeshComponent || !OriginalGlassMaterial)
	{
		return;
	}

	MeshComponent->SetMaterial(1, OriginalGlassMaterial);
	GlassStatusMID = nullptr;
}

FString AApartmentActor::ResolveExternalIdFromMeshName() const
{
	const UStaticMeshComponent* MeshComponent = GetStaticMeshComponent();
	if (!MeshComponent || !MeshComponent->GetStaticMesh())
	{
		return FString();
	}

	return FApartmentIdMapping::MeshNameToExternalId(MeshComponent->GetStaticMesh()->GetName());
}
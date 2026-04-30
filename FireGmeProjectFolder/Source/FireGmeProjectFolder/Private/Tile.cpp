#include "Tile.h"
#include "UObject/ConstructorHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"


// Sets default values
ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
	bIsSelected = false;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	RootComponent = TileMesh;

	AlertIndicator = CreateDefaultSubobject<UBillboardComponent>(TEXT("AlertIndicator"));
	AlertIndicator->SetupAttachment(RootComponent);
	AlertIndicator->SetHiddenInGame(true);
	AlertIndicator->SetVisibility(false);
	AlertIndicator->SetRelativeLocation(FVector(0.f, 0.f, AlertIndicatorZOffset));
	AlertIndicator->SetUsingAbsoluteRotation(true);
	AlertIndicator->SetCollisionObjectType(ECC_WorldDynamic);
	AlertIndicator->SetCollisionResponseToAllChannels(ECR_Ignore);
	AlertIndicator->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	AlertIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AlertTurnsText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("AlertTurnsText"));
	AlertTurnsText->SetupAttachment(AlertIndicator);
	AlertTurnsText->SetUsingAbsoluteRotation(true);
	AlertTurnsText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	AlertTurnsText->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	AlertTurnsText->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
	AlertTurnsText->SetWorldSize(54.0f);
	AlertTurnsText->SetTextRenderColor(FColor::White);
	AlertTurnsText->SetHiddenInGame(true);
	AlertTurnsText->SetVisibility(false);
	AlertTurnsText->SetText(FText::AsNumber(AlertTurnsRemaining));
	AlertTurnsText->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

	static ConstructorHelpers::FObjectFinder<UTexture2D> AlertSpriteObj(
		TEXT("/Game/FireGame/Alerts/T_red_pin_shape.T_red_pin_shape"));
	if (AlertSpriteObj.Succeeded())
	{
		AlertIndicatorTexture = AlertSpriteObj.Object;
		AlertIndicator->SetSprite(AlertIndicatorTexture);
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExtinguishEffectObj(
		TEXT("/Game/FireGame/Tiles/Water_Materials/NS_Extinguish.NS_Extinguish"));
	if (ExtinguishEffectObj.Succeeded())
	{
		ExtinguishEffect = ExtinguishEffectObj.Object;
	}
}

void ATile::Select()
{
	bIsSelected = true;
	OnSelected();
}

void ATile::Deselect()
{
	bIsSelected = false;
	OnDeselected();
}

void ATile::OnSelected_Implementation()
{
	// Override in Blueprint — highlight unit, show movement range, etc.
}

void ATile::OnDeselected_Implementation()
{
	// Override in Blueprint — remove visual feedback
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoSyncGridCoordinates)
	{
		SyncGridCoordinatesFromWorld();
	}

	SetAlertTurnsRemaining(AlertTurnsRemaining);
}

void ATile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bAutoSyncGridCoordinates)
	{
		SyncGridCoordinatesFromWorld();
	}
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAlertTurnsTextFacingCamera();
}

void ATile::Ignite()
{
	if (bIsBurnable && !bIsBurning)
	{
		bIsBurning = true;
	}
}

void ATile::Extinguish()
{
	bIsBurning = false;
	bWillIgniteNextTurn = false;

	if (!ExtinguishEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExtinguishEffect is null on tile: %s"), *GetName());
		return;
	}

	UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		ExtinguishEffect,
		GetActorLocation(),
		FRotator::ZeroRotator,
		FVector(1.0f),
		true,
		true,
		ENCPoolMethod::None,
		true);

	if (!SpawnedEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn ExtinguishEffect on tile: %s"), *GetName());
	}
}

bool ATile::IsAlertIndicatorVisible() const
{
	return AlertIndicator && AlertIndicator->IsVisible();
}

void ATile::SetAlertIndicatorVisible(bool bVisible)
{
	if (!AlertIndicator)
	{
		return;
	}

	AlertIndicator->SetHiddenInGame(!bVisible);
	AlertIndicator->SetVisibility(bVisible);
	AlertIndicator->SetCollisionEnabled(bVisible ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	AlertIndicator->SetRelativeLocation(FVector(0.f, 0.f, AlertIndicatorZOffset));

	if (AlertTurnsText)
	{
		const bool bShowTurns = bVisible && AlertTurnsRemaining > 0;
		AlertTurnsText->SetHiddenInGame(!bShowTurns);
		AlertTurnsText->SetVisibility(bShowTurns);
	}
}

void ATile::SetAlertTurnsRemaining(int32 InTurnsRemaining)
{
	AlertTurnsRemaining = FMath::Max(0, InTurnsRemaining);

	if (AlertTurnsText)
	{
		AlertTurnsText->SetText(FText::AsNumber(AlertTurnsRemaining));

		const bool bShowTurns = IsAlertIndicatorVisible() && AlertTurnsRemaining > 0;
		AlertTurnsText->SetHiddenInGame(!bShowTurns);
		AlertTurnsText->SetVisibility(bShowTurns);
	}
}

void ATile::ReduceCommunityHealthCost()
{
	if (CommunityHealthCost > 2) {
		CommunityHealthCost--;
		UE_LOG(LogTemp, Log, TEXT("New Community Health Cost: %d"), CommunityHealthCost);
	}
}

int32 ATile::GetTileID()
{
	return TileID;
}

bool ATile::ApplyDataFromRowName(FName RowName)
{
	if (!TileDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("TileDataTable is null on tile at (%d,%d,%d)"),
			GridCoordinates.X, GridCoordinates.Y, GridCoordinates.Z);
		return false;
	}

	static const FString Context(TEXT("TileDataLookup"));
	const FTileDataRow* Row = TileDataTable->FindRow<FTileDataRow>(RowName, Context);
	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("No row named %s found in DT_Tile"), *RowName.ToString());
		return false;
	}

	TileRowName = RowName;
	TileID = Row->ID;
	bIsBurnable = Row->Is_Burnable;
	CurrentFireHealth = Row->Tile_Health;
	CommunityHealthCost = Row->Community_Health_Cost;

	BaseMaterial = Row->Material;
	PreFireMaterial = Row->PreFireMaterial;
	BurningMaterial = Row->BurningMaterial;

	TileModels = Row->Models;

	// Fallback for if mesh table is empty
	if (TileModels.Num() == 0 && Row->Model != nullptr)
	{
		TileModels.Add(Row->Model);
	}

	// Does not directly set GridCoordinates from the row anymore.
	// BP visuals / UpdateDataTable set world location using hex spacing defined in the BP.
	UpdateTileVisuals();

	if (bAutoSyncGridCoordinates)
	{
		SyncGridCoordinatesFromWorld();
	}

	return true;
}

bool ATile::ApplyDataFromID(int32 InTileID)
{
	if (!TileDataTable)
	{
		return false;
	}

	static const FString Context(TEXT("TileDataLookup"));
	const TArray<FName> RowNames = TileDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		const FTileDataRow* Row = TileDataTable->FindRow<FTileDataRow>(RowName, Context);
		if (Row && Row->ID == InTileID)
		{
			TileRowName = RowName;
			TileID = Row->ID;
			bIsBurnable = Row->Is_Burnable;
			CurrentFireHealth = Row->Tile_Health;
			CommunityHealthCost = Row->Community_Health_Cost;
			CurrentMesh = Row->Model;
			if (TileMesh && CurrentMesh)
			{
				TileMesh->SetStaticMesh(CurrentMesh);
			}

			BaseMaterial = Row->Material;
			PreFireMaterial = Row->PreFireMaterial;
			BurningMaterial = Row->BurningMaterial;

			TileModels = Row->Models;

			// Fallback
			if (TileModels.Num() == 0 && Row->Model != nullptr)
			{
				TileModels.Add(Row->Model);
			}

			UpdateTileVisuals();

			if (bAutoSyncGridCoordinates)
			{
				SyncGridCoordinatesFromWorld();
			}

			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No row with ID %d found in DT_Tile"), InTileID);
	return false;
}

FIntVector ATile::ConvertWorldToGridCoordinates_Implementation(const FVector& WorldLocation) const
{
	// Default fallback only. Override this in BP_Tile with real world coordinates. Not overriding in BP_Tile will cause this to break.
	return FIntVector(
		FMath::RoundToInt(WorldLocation.X),
		FMath::RoundToInt(WorldLocation.Y),
		FMath::RoundToInt(WorldLocation.Z)
	);
}

FVector ATile::ConvertGridToWorldCoordinates_Implementation(const FIntVector& InGridCoordinates) const
{
	// Default fallback only. Override this in BP_Tile later to reverse conversion (in BP vs in C++).
	return FVector(
		static_cast<float>(InGridCoordinates.X),
		static_cast<float>(InGridCoordinates.Y),
		static_cast<float>(InGridCoordinates.Z)
	);
}

void ATile::SyncGridCoordinatesFromWorld()
{
	GridCoordinates = ConvertWorldToGridCoordinates(GetActorLocation());

	UE_LOG(LogTemp, Verbose, TEXT("Synced GridCoordinates to (%d, %d, %d) from world location (%s)"),
		GridCoordinates.X,
		GridCoordinates.Y,
		GridCoordinates.Z,
		*GetActorLocation().ToString());
}

void ATile::SyncWorldFromGrid()
{
	const FVector NewWorldLocation = ConvertGridToWorldCoordinates(GridCoordinates);
	SetActorLocation(NewWorldLocation);

	UE_LOG(LogTemp, Verbose, TEXT("Synced world location to (%s) from GridCoordinates (%d, %d, %d)"),
		*NewWorldLocation.ToString(),
		GridCoordinates.X,
		GridCoordinates.Y,
		GridCoordinates.Z);
}

void ATile::UpdateAlertTurnsTextFacingCamera() const
{
	if (!bAlertTurnsTextFacesCamera || !AlertTurnsText || !AlertTurnsText->IsVisible())
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
	if (!PlayerController)
	{
		return;
	}

	FVector ViewLocation = FVector::ZeroVector;
	FRotator ViewRotation = FRotator::ZeroRotator;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TextLocation = AlertTurnsText->GetComponentLocation();
	const FVector ToCamera = ViewLocation - TextLocation;
	if (ToCamera.IsNearlyZero())
	{
		return;
	}

	FRotator FaceCameraRotation = ToCamera.Rotation();

	FaceCameraRotation.Yaw += 0.0f;
	AlertTurnsText->SetWorldRotation(FaceCameraRotation);
}
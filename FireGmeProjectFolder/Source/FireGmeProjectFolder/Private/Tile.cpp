#include "Tile.h"
#include "UObject/ConstructorHelpers.h" 

// Sets default values
ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = true;
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

	static ConstructorHelpers::FObjectFinder<UTexture2D> AlertSpriteObj(
		TEXT("/Game/FireGame/UI/T_Alert-Icon.T_Alert-Icon"));
	if (AlertSpriteObj.Succeeded())
	{
		AlertIndicatorTexture = AlertSpriteObj.Object;
		AlertIndicator->SetSprite(AlertIndicatorTexture);
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
}

bool ATile::IsAlertIndicatorVisible() const
{
	return AlertIndicator && AlertIndicator->IsVisible();
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
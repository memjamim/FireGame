#include "Unit.h"
#include "Tile.h"
#include "TileManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
	UnitMesh->SetupAttachment(Root);

	CurrentStamina = 100;
	CurrentTile = nullptr;
	TileManager = nullptr;
	GridCoordinates = FIntVector::ZeroValue;
	bIsSelected = false;
	bHasActedThisTurn = false;
}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
	Super::BeginPlay();

	// Cache the TileManager reference
	TileManager = FindTileManager();

	if (!TileManager)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: No ATileManager found in the level."), *GetName());
		return;
	}

	// Link to the tile at our GridCoordinates (default 0,0,0)
	if (!CurrentTile)
	{
		if (ATile* const* FoundTile = TileManager->TileLookup.Find(GridCoordinates))
		{
			SetCurrentTile(*FoundTile);

			UE_LOG(LogTemp, Warning, TEXT("%s: Spawned on tile at (%d, %d, %d) — TileID: %d"),
				*GetName(),
				GridCoordinates.X, GridCoordinates.Y, GridCoordinates.Z,
				CurrentTile->TileID);

			UE_LOG(LogTemp, Warning, TEXT("  Name: %s | ID: %d | Description: %s"),
				*UnitData.Name, UnitData.ID, *UnitData.Description);

			UE_LOG(LogTemp, Warning, TEXT("  Stamina: %d (Current: %d) | Movement Type: %d"),
				UnitData.Stamina, CurrentStamina, UnitData.Unit_Movement_Type);

			UE_LOG(LogTemp, Warning, TEXT("  Action Cost: %d | Turns To Deploy: %d"),
				UnitData.Action_Cost, UnitData.Turns_To_Deploy);

			UE_LOG(LogTemp, Warning, TEXT("  Coordinates: (%d, %d, %d)"),
				UnitData.Coordinates.X, UnitData.Coordinates.Y, UnitData.Coordinates.Z);

			UE_LOG(LogTemp, Warning, TEXT("  Model: %s | Material: %s"),
				UnitData.Model.IsNull() ? TEXT("None") : *UnitData.Model.GetAssetName(),
				UnitData.Material.IsNull() ? TEXT("None") : *UnitData.Material.GetAssetName());

			UE_LOG(LogTemp, Warning, TEXT("  Unit_BP: %s"),
				UnitData.Unit_BP ? *UnitData.Unit_BP->GetName() : TEXT("None"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: No tile found at (%d, %d, %d)."),
				*GetName(),
				GridCoordinates.X, GridCoordinates.Y, GridCoordinates.Z);
		}
	}
}

// Called every frame
void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Data Table Functions

bool AUnit::ApplyDataFromRowName(FName RowName)
{
	if (!UnitDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: UnitDataTable is not set."), *GetName());
		return false;
	}

	static const FString Context(TEXT("UnitDataLookup"));
	const FUnitData* Row = UnitDataTable->FindRow<FUnitData>(RowName, Context);
	if (!Row)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: No row named '%s' in UnitDataTable."),
			*GetName(), *RowName.ToString());
		return false;
	}

	UnitData = *Row;
	CurrentStamina = UnitData.Stamina;
	GridCoordinates = UnitData.Coordinates;

	// Load and apply mesh if specified
	if (!UnitData.Model.IsNull())
	{
		UStaticMesh* LoadedMesh = UnitData.Model.LoadSynchronous();
		if (LoadedMesh && UnitMesh)
		{
			UnitMesh->SetStaticMesh(LoadedMesh);
		}
	}

	// Load and apply material if specified
	if (!UnitData.Material.IsNull())
	{
		UMaterialInterface* LoadedMaterial = UnitData.Material.LoadSynchronous();
		if (LoadedMaterial && UnitMesh)
		{
			UnitMesh->SetMaterial(0, LoadedMaterial);
		}
	}

	// Snap to the tile at the row's coordinates
	if (TileManager)
	{
		if (ATile* const* FoundTile = TileManager->TileLookup.Find(GridCoordinates))
		{
			SetCurrentTile(*FoundTile);
		}
	}

	return true;
}

// Position Functions

void AUnit::SetCurrentTile(ATile* NewTile)
{
	CurrentTile = NewTile;

	if (CurrentTile)
	{
		GridCoordinates = CurrentTile->GridCoordinates;
		SetActorLocation(CurrentTile->GetActorLocation());
	}
}

TArray<ATile*> AUnit::GetAdjacentTiles() const
{
	if (TileManager)
	{
		return TileManager->GetNeighborTiles(GridCoordinates);
	}

	return TArray<ATile*>();
}

// Movement Functions

bool AUnit::MoveToTile(FIntVector TargetCoordinates)
{
	if (!TileManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: No TileManager — cannot move."), *GetName());
		return false;
	}

	ATile* const* FoundTile = TileManager->TileLookup.Find(TargetCoordinates);
	if (!FoundTile || !IsValid(*FoundTile))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: No tile at (%d, %d, %d)."),
			*GetName(),
			TargetCoordinates.X, TargetCoordinates.Y, TargetCoordinates.Z);
		return false;
	}

	ATile* TargetTile = *FoundTile;

	if (!CanMoveToTile(TargetTile))
	{
		return false;
	}

	SetCurrentTile(TargetTile);
	OnMoveComplete();
	return true;
}

bool AUnit::CanMoveToTile_Implementation(ATile* TargetTile)
{
	// Base check — override in Blueprint for range, terrain, occupied checks
	return IsValid(TargetTile);
}

void AUnit::OnMoveComplete_Implementation()
{
	// Override in Blueprint for custom behavior
}

// Selection functions

void AUnit::Select()
{
	bIsSelected = true;
	OnSelected();
}

void AUnit::Deselect()
{
	bIsSelected = false;
	OnDeselected();
}

void AUnit::OnSelected_Implementation()
{
	// Override in Blueprint — highlight unit, show movement range, etc.
}

void AUnit::OnDeselected_Implementation()
{
	// Override in Blueprint — remove visual feedback
}

// Turn functions

void AUnit::StartTurn()
{
	bHasActedThisTurn = false;
	RestoreStamina(UnitData.Stamina);
}

void AUnit::EndTurn()
{
	bHasActedThisTurn = true;
}


// Ability functions

bool AUnit::CanUseAbility(int32 AbilityIndex)
{
	if (bHasActedThisTurn)
	{
		return false;
	}

	// Extend in Blueprint per-unit-type
	return true;
}

void AUnit::UseAbility_Implementation(int32 AbilityIndex, ATile* TargetTile)
{
	// Override in Blueprint — Helicopter, Residential FF, Wildland FF each have unique abilities
}

TArray<FString> AUnit::GetAvailableAbilities_Implementation()
{
	return TArray<FString>();
}

// Stamina functions

void AUnit::ConsumeStamina(int32 Amount)
{
	CurrentStamina = FMath::Max(CurrentStamina - Amount, 0);
}

void AUnit::RestoreStamina(int32 Amount)
{
	CurrentStamina = FMath::Min(CurrentStamina + Amount, UnitData.Stamina);
}

bool AUnit::HasEnoughStamina(int32 Required) const
{
	return CurrentStamina >= Required;
}

// Helper functions

int32 AUnit::GetCubeDistance(FIntVector CubeA, FIntVector CubeB)
{
	FIntVector Diff = CubeA - CubeB;
	return (FMath::Abs(Diff.X) + FMath::Abs(Diff.Y) + FMath::Abs(Diff.Z)) / 2;
}

bool AUnit::IsValidCubeCoordinate(FIntVector Cube)
{
	return (Cube.X + Cube.Y + Cube.Z) == 0;
}

ATileManager* AUnit::FindTileManager() const
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATileManager::StaticClass(), Found);

	if (Found.Num() > 0)
	{
		return Cast<ATileManager>(Found[0]);
	}

	return nullptr;
}
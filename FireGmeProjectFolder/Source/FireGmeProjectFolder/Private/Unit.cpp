// Fill out your copyright notice in the Description page of Project Settings.

#include "Unit.h"
#include "Tile.h"

// Sets default values
AUnit::AUnit()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
    UnitMesh->SetupAttachment(Root);

    CurrentStamina = 100;
    CurrentTile = nullptr;
    CubeCoordinates = FVector::ZeroVector;
    bIsSelected = false;
    bHasActedThisTurn = false;
}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AUnit::InitializeFromDataTable(FName RowName)
{
    if (!UnitDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("UnitDataTable is not set for %s"), *GetName());
        return;
    }

    FUnitData* Row = UnitDataTable->FindRow<FUnitData>(RowName, TEXT(""));
    if (Row)
    {
        UnitData = *Row;
        CurrentStamina = UnitData.Stamina;
        CubeCoordinates = UnitData.Coordinates;

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
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Could not find row %s in UnitDataTable"), *RowName.ToString());
    }
}

bool AUnit::CanMoveToTile(ATile* TargetTile)
{
    if (!TargetTile)
    {
        return false;
    }

    // Add additional checks here, such as checking if the tile is occupied, if it's within movement range, 
    // terrain type, etc.
    return true;
}

bool AUnit::MoveToTile(ATile* TargetTile)
{
    if (!CanMoveToTile(TargetTile))
    {
        return false;
    }

    // Update tile reference
    ATile* PreviousTile = CurrentTile;
    CurrentTile = TargetTile;

    // Move to tile's location
    SetActorLocation(TargetTile->GetActorLocation());

    // Consume stamina
    ConsumeStamina(UnitData.Action_Cost);

    // Trigger completion event (Blueprint can override for animations, etc.)
    OnMoveComplete();

    return true;
}

void AUnit::OnMoveComplete_Implementation()
{
    // Default implementation - override in Blueprint for custom behavior
}

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
    // Default: Visual feedback (override in Blueprint)
    // Example: Highlight the unit, show movement range
}

void AUnit::OnDeselected_Implementation()
{
    // Default: Remove visual feedback
}

void AUnit::StartTurn()
{
    bHasActedThisTurn = false;
    RestoreStamina(UnitData.Stamina);
}

void AUnit::EndTurn()
{
    bHasActedThisTurn = true;
}

bool AUnit::CanUseAbility(int32 AbilityIndex)
{
    if (bHasActedThisTurn)
    {
        return false;
    }

    // Check stamina, cooldowns, etc. (implement in Blueprint)
    return true;
}

void AUnit::UseAbility_Implementation(int32 AbilityIndex, ATile* TargetTile)
{
    // Override in Blueprint for specific abilities
}

TArray<FString> AUnit::GetAvailableAbilities_Implementation()
{
    // Default implementation - override in Blueprint for custom behavior
    return TArray<FString>();
}

TArray<ATile*> AUnit::GetTilesInRange(int32 Range)
{
    TArray<ATile*> TilesInRange;

    // Implementation would query your tile grid manager
    // Pseudocode:
    // for each tile in grid:
    //     if GetCubeDistance(CubeCoordinates, tile.CubeCoordinates) <= Range:
    //         TilesInRange.Add(tile)

    return TilesInRange;
}

int32 AUnit::GetCubeDistance(FVector CubeA, FVector CubeB)
{
    // Distance in cube coordinates: (|dx| + |dy| + |dz|) / 2
    FVector Diff = CubeA - CubeB;
    return (FMath::Abs(Diff.X) + FMath::Abs(Diff.Y) + FMath::Abs(Diff.Z)) / 2;
}

bool AUnit::IsValidCubeCoordinate(FVector Cube)
{
    // In cube coordinates, x + y + z should always equal 0
    return FMath::IsNearlyZero(Cube.X + Cube.Y + Cube.Z);
}

TArray<FVector> AUnit::GetNeighbors(FVector CubeCoord)
{
    TArray<FVector> Directions = {
        FVector(-1, 0, 1),   // North
        FVector(0, -1, 1),   // Northeast
        FVector(1, -1, 0),   // Southeast
        FVector(1, 0, -1),   // South
        FVector(0, 1, -1),   // Southwest
        FVector(-1, 1, 0)    // Northwest
    };

    TArray<FVector> Neighbors;
    for (const FVector& Dir : Directions)
    {
        Neighbors.Add(CubeCoord + Dir);
    }

    return Neighbors;
}

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
#include "TileManager.h"
#include "Tile.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

ATileManager::ATileManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATileManager::BeginPlay()
{
	Super::BeginPlay();

	// Auto-register all tiles already placed in the level
	TArray<AActor*> FoundTiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATile::StaticClass(), FoundTiles);

	for (AActor* Actor : FoundTiles)
	{
		if (ATile* Tile = Cast<ATile>(Actor))
		{
			RegisterTile(Tile);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("TileManager initialized with %d tiles."), RegisteredTiles.Num());
}

void ATileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATileManager::RegisterTile(ATile* Tile)
{
	if (!IsValid(Tile))
	{
		return;
	}

	if (RegisteredTiles.Contains(Tile))
	{
		return;
	}

	RegisteredTiles.Add(Tile);
	TileLookup.Add(Tile->GridCoordinates, Tile);

	UE_LOG(LogTemp, Verbose, TEXT("Registered tile ID %d at (%d, %d, %d)"),
		Tile->TileID,
		Tile->GridCoordinates.X,
		Tile->GridCoordinates.Y,
		Tile->GridCoordinates.Z);
}

void ATileManager::UnregisterTile(ATile* Tile)
{
	if (!IsValid(Tile))
	{
		return;
	}

	RegisteredTiles.Remove(Tile);
	TileLookup.Remove(Tile->GridCoordinates);
}

void ATileManager::ExecuteFireTurn()
{
	// Snapshot first so newly ignited tiles don't also act this same fire turn
	TArray<ATile*> BurningTilesAtStart;

	for (ATile* Tile : RegisteredTiles)
	{
		if (IsValid(Tile) && Tile->bIsBurning)
		{
			BurningTilesAtStart.Add(Tile);
		}
	}

	if (BurningTilesAtStart.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Fire turn: no burning tiles."));
		return;
	}

	TSet<ATile*> TilesToIgnite;

	// 1) Existing fires burn down and choose spread targets
	for (ATile* BurningTile : BurningTilesAtStart)
	{
		ProcessBurningTile(BurningTile, TilesToIgnite);
	}

	// 2) After all current fires processed, ignite new tiles
	for (ATile* TileToIgnite : TilesToIgnite)
	{
		if (IsValid(TileToIgnite) && TileToIgnite->bIsBurnable && !TileToIgnite->bIsBurning)
		{
			IgniteTile(TileToIgnite);

			UE_LOG(LogTemp, Log, TEXT("Tile ignited: ID %d at (%d, %d, %d)"),
				TileToIgnite->TileID,
				TileToIgnite->GridCoordinates.X,
				TileToIgnite->GridCoordinates.Y,
				TileToIgnite->GridCoordinates.Z);
		}
	}
}

void ATileManager::IgniteTile(ATile* Tile)
{
	if (!IsValid(Tile))
	{
		return;
	}

	if (!Tile->bIsBurnable || Tile->bIsBurning)
	{
		return;
	}

	Tile->Ignite();
	Tile->UpdateTileVisuals();
}

TArray<ATile*> ATileManager::GetNeighborTiles(const FIntVector& Center) const
{
	// Cube-coordinate neighbor directions for hex grids
	static const TArray<FIntVector> Directions =
	{
		FIntVector(1, -1,  0),
		FIntVector(1,  0, -1),
		FIntVector(0,  1, -1),
		FIntVector(-1,  1,  0),
		FIntVector(-1,  0,  1),
		FIntVector(0, -1,  1)
	};

	TArray<ATile*> Neighbors;
	Neighbors.Reserve(6);

	for (const FIntVector& Dir : Directions)
	{
		const FIntVector NeighborCoord = Center + Dir;

		if (ATile* const* FoundTile = TileLookup.Find(NeighborCoord))
		{
			if (IsValid(*FoundTile))
			{
				Neighbors.Add(*FoundTile);
			}
		}
	}

	return Neighbors;
}

void ATileManager::ProcessBurningTile(ATile* BurningTile, TSet<ATile*>& OutTilesToIgnite)
{
	if (!IsValid(BurningTile) || !BurningTile->bIsBurning)
	{
		return;
	}

	// Pick spread targets before spreading burn in this implementation.
	GetSpreadTargetsForTile(BurningTile, OutTilesToIgnite);

	BurningTile->CurrentFireHealth -= 1;

	UE_LOG(LogTemp, Log, TEXT("Burning tile ID %d at (%d, %d, %d) now has fire health %d"),
		BurningTile->TileID,
		BurningTile->GridCoordinates.X,
		BurningTile->GridCoordinates.Y,
		BurningTile->GridCoordinates.Z,
		BurningTile->CurrentFireHealth);

	if (BurningTile->CurrentFireHealth <= 0)
	{
		BurnOutTile(BurningTile);
	}
	else
	{
		BurningTile->UpdateTileVisuals();
	}
}

void ATileManager::BurnOutTile(ATile* Tile)
{
	if (!IsValid(Tile))
	{
		return;
	}

	Tile->bIsBurning = false;
	Tile->bIsBurnable = false;

	if (!Tile->bHasBeenDestroyedByFire)
	{
		ApplyCommunityDamage(Tile);
		Tile->bHasBeenDestroyedByFire = true;
	}

	// Swap to charred tile
	Tile->TileID = CharredTileID;

	// Charred tiles should never reignite
	Tile->CurrentFireHealth = 0;

	Tile->ApplyDataFromID(CharredTileID);

	UE_LOG(LogTemp, Warning, TEXT("Tile burned out and became charred at (%d, %d, %d)"),
		Tile->GridCoordinates.X,
		Tile->GridCoordinates.Y,
		Tile->GridCoordinates.Z);
}

void ATileManager::ApplyCommunityDamage(ATile* Tile)
{
	if (!IsValid(Tile))
	{
		return;
	}

	CommunityHealth = FMath::Clamp(CommunityHealth - Tile->CommunityHealthCost, 0, 100);

	UE_LOG(LogTemp, Warning, TEXT("Community Health reduced by %d. New value: %d"),
		Tile->CommunityHealthCost,
		CommunityHealth);
}

void ATileManager::GetSpreadTargetsForTile(ATile* SourceTile, TSet<ATile*>& OutTilesToIgnite) const
{
	if (!IsValid(SourceTile))
	{
		return;
	}

	switch (SourceTile->TileID)
	{
	case 1: // Grass example
		GetGrassSpreadTargets(SourceTile, OutTilesToIgnite);
		break;

	case 2: // Mountain example
		// Mountains shouldn't burn if configured correctly, but if they somehow are burning,
		// they spread to nothing.
		break;

	default:
		GetDefaultSpreadTargets(SourceTile, OutTilesToIgnite);
		break;
	}
}

// Currently spreads to all avalible targets rather than being randomized
void ATileManager::GetDefaultSpreadTargets(ATile* SourceTile, TSet<ATile*>& OutTilesToIgnite) const
{
	if (!IsValid(SourceTile))
	{
		return;
	}

	const TArray<ATile*> Neighbors = GetNeighborTiles(SourceTile->GridCoordinates);

	for (ATile* Neighbor : Neighbors)
	{
		if (!IsValid(Neighbor))
		{
			continue;
		}

		if (Neighbor->bIsBurnable && !Neighbor->bIsBurning)
		{
			OutTilesToIgnite.Add(Neighbor);
		}
	}
}

void ATileManager::GetGrassSpreadTargets(ATile* SourceTile, TSet<ATile*>& OutTilesToIgnite) const
{
	if (!IsValid(SourceTile))
	{
		return;
	}

	const TArray<ATile*> Neighbors = GetNeighborTiles(SourceTile->GridCoordinates);

	for (ATile* Neighbor : Neighbors)
	{
		if (!IsValid(Neighbor))
		{
			continue;
		}

		// Example rule: grass only spreads to adjacent grass tiles
		if (Neighbor->TileID == GrassTileID && Neighbor->bIsBurnable && !Neighbor->bIsBurning)
		{
			OutTilesToIgnite.Add(Neighbor);
		}
	}
}
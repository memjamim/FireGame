#include "TileManager.h"
#include "Tile.h"
#include "ProceduralGeneration.h"
#include "GameManager.h"
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

	UE_LOG(LogTemp, Error, TEXT("TILE MANAGER BEGIN PLAY RUNNING"));
	// Auto-register all tiles already placed in the level
	// TArray<AActor*> FoundTiles;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATile::StaticClass(), FoundTiles);

	//for (AActor* Actor : FoundTiles)
	//{
	//	if (ATile* Tile = Cast<ATile>(Actor))
	//	{
	//		RegisterTile(Tile);
	//	}
	//}

	ProceduralGeneration::GenerateMap(GetWorld(), TileClass, 150, 1, 4, 0, this); // Generates the map procedurally.

	if (RegisteredTiles.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, RegisteredTiles.Num() - 1);
		/* NOTE FOR KYLE: We eventually want the fire to start not at a random index, but at a random Forest tile.
		   Furthermore, we want the fire to start somewhat away from Residential tiles (and others of the sorts) so
		   as to provide the player a fair opportunity to get setup. */
		ATile* StartTile = RegisteredTiles[RandomIndex]; 

		IgniteTile(StartTile);

		UE_LOG(LogTemp, Warning, TEXT("Initial fire started at (%d, %d, %d)"),
			StartTile->GridCoordinates.X,
			StartTile->GridCoordinates.Y,
			StartTile->GridCoordinates.Z);
	}

	// Auto-find GameManager if not set
	if (!GameManager)
	{
		TArray<AActor*> FoundManagers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameManager::StaticClass(), FoundManagers);
		if (FoundManagers.Num() > 0)
		{
			GameManager = Cast<AGameManager>(FoundManagers[0]);
		}
	}

	ClearPreFireIndicators();

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

void ATileManager::ClearPreFireIndicators()
{
	for (ATile* Tile : RegisteredTiles)
	{
		if (!IsValid(Tile))
		{
			continue;
		}

		if (Tile->bWillIgniteNextTurn)
		{
			Tile->bWillIgniteNextTurn = false;
			Tile->UpdateTileVisuals();
		}
	}
}

int32 ATileManager::GetWindDirectionIndex() const
{
	if (!IsValid(GameManager))
	{
		return -1;
	}

	// Expected 0..5
	const int32 Dir = static_cast<int32>(GameManager->WindDirection);
	return (Dir >= 0 && Dir < 6) ? Dir : -1;
}

void ATileManager::ExecuteFireTurn()
{
	// Track tiles that ignite at the start of this fire step.
	TSet<ATile*> NewlyIgnitedThisTurn;

	// 0) Ignite tiles that were previewed last turn
	for (ATile* Tile : RegisteredTiles)
	{
		if (!IsValid(Tile)) continue;

		if (Tile->bWillIgniteNextTurn)
		{
			Tile->bWillIgniteNextTurn = false; // consume preview

			if (Tile->bIsBurnable && !Tile->bIsBurning)
			{
				IgniteTile(Tile);
				NewlyIgnitedThisTurn.Add(Tile);
			}
			else
			{
				// ensure indicator clears visually even if ignition is blocked
				Tile->UpdateTileVisuals();
			}
		}
	}

	// 1) Create snapshot of tiles burning right now (after igniting)
	TArray<ATile*> BurningNow;
	for (ATile* Tile : RegisteredTiles)
	{
		if (IsValid(Tile) && Tile->bIsBurning)
		{
			BurningNow.Add(Tile);
		}
	}

	// 2) Create preview for the next turn from BurningNow (includes newly ignited tiles)
	TSet<ATile*> TilesToMarkNextTurn;
	for (ATile* BurningTile : BurningNow)
	{
		GetSpreadTargetsForTile(BurningTile, TilesToMarkNextTurn);
	}

	// 3) Advance fire (health tick / burnout) for tiles that were already burning
	// before the current fire step (not NewlyIgnitedThisTurn).
	for (ATile* BurningTile : BurningNow)
	{
		if (!IsValid(BurningTile)) continue;

		if (NewlyIgnitedThisTurn.Contains(BurningTile))
		{
			// Set newly ingited visuals
			BurningTile->UpdateTileVisuals();
			continue;
		}

		BurningTile->CurrentFireHealth -= 1;

		if (BurningTile->CurrentFireHealth <= 0)
		{
			BurnOutTile(BurningTile);
		}
		else
		{
			BurningTile->UpdateTileVisuals();
		}
	}

	// 4) Clear any leftover prefire flags
	for (ATile* Tile : RegisteredTiles)
	{
		if (!IsValid(Tile)) continue;

		if (Tile->bWillIgniteNextTurn)
		{
			Tile->bWillIgniteNextTurn = false;
			Tile->UpdateTileVisuals();
		}
	}

	// 5) Apply new preview marks
	for (ATile* TileToMark : TilesToMarkNextTurn)
	{
		if (!IsValid(TileToMark)) continue;

		if (TileToMark->bIsBurnable && !TileToMark->bIsBurning)
		{
			TileToMark->bWillIgniteNextTurn = true;
			TileToMark->UpdateTileVisuals();
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

	// If it was going to ignite next turn for some reason, clear that
	if (Tile->bWillIgniteNextTurn)
	{
		Tile->bWillIgniteNextTurn = false;
	}

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

	UE_LOG(LogTemp, Log, TEXT("Tile burned out and became charred at (%d, %d, %d)"),
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

	UE_LOG(LogTemp, Log, TEXT("Community Health reduced by %d. New value: %d"),
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

// Default rule:
// - Wind-direction neighbor is always targeted (if exists and is eligible).
// - Other 5 neighbors roll (NonWindSpreadNumerator / NonWindSpreadDenominator).
void ATileManager::GetDefaultSpreadTargets(ATile* SourceTile, TSet<ATile*>& OutTilesToIgnite) const
{
	if (!IsValid(SourceTile))
	{
		return;
	}

	// Matches GetNeighborTiles() direction ordering
	static const TArray<FIntVector> Directions =
	{
		FIntVector(1, -1,  0),
		FIntVector(1,  0, -1),
		FIntVector(0,  1, -1),
		FIntVector(-1,  1,  0),
		FIntVector(-1,  0,  1),
		FIntVector(0, -1,  1)
	};

	const int32 WindDir = GetWindDirectionIndex();

	const int32 Den = FMath::Max(1, NonWindSpreadDenominator);
	const int32 Num = FMath::Clamp(NonWindSpreadNumerator, 0, Den);

	for (int32 i = 0; i < Directions.Num(); ++i)
	{
		const FIntVector NeighborCoord = SourceTile->GridCoordinates + Directions[i];

		ATile* Neighbor = nullptr;
		if (ATile* const* Found = TileLookup.Find(NeighborCoord))
		{
			Neighbor = *Found;
		}

		if (!IsValid(Neighbor))
		{
			continue;
		}

		// Only adds tiles that can actually be planned to ignite next turn
		if (!Neighbor->bIsBurnable || Neighbor->bIsBurning || Neighbor->bWillIgniteNextTurn)
		{
			continue;
		}

		// Always spreads in wind direction
		if (WindDir != -1 && i == WindDir)
		{
			OutTilesToIgnite.Add(Neighbor);
			continue;
		}

		// Non-wind neighbors have a roll
		// Roll 1..Den; ignite if Roll <= Num (Currently always 3/6, can be modified later for 
		// difficulty adjustment / different difficulty modes).
		const int32 Roll = FMath::RandRange(1, Den);
		if (Roll <= Num)
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
		if (/*Neighbor->TileID == GrassTileID && */Neighbor->bIsBurnable && !Neighbor->bIsBurning)
		{
			OutTilesToIgnite.Add(Neighbor);
		}
	}
}

void ATileManager::UpdateFirePreview()
{
	for (ATile* Tile : RegisteredTiles)
	{
		if (!IsValid(Tile)) continue;

		if (Tile->bWillIgniteNextTurn)
		{
			Tile->bWillIgniteNextTurn = false;
			Tile->UpdateTileVisuals();
		}
	}

	// get preview targets from tiles currently burning
	TSet<ATile*> TilesToMark;
	for (ATile* Tile : RegisteredTiles)
	{
		if (!IsValid(Tile)) continue;
		if (!Tile->bIsBurning) continue;

		GetSpreadTargetsForTile(Tile, TilesToMark);
	}

	// Mark preview
	for (ATile* T : TilesToMark)
	{
		if (!IsValid(T)) continue;

		if (T->bIsBurnable && !T->bIsBurning)
		{
			T->bWillIgniteNextTurn = true;
			T->UpdateTileVisuals();
		}
	}
}
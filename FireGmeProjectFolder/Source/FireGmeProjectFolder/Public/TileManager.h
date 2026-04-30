#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileManager.generated.h"

class ATile;
class UDataTable;
class AGameManager;

UCLASS()
class FIREGMEPROJECTFOLDER_API ATileManager : public AActor
{
	GENERATED_BODY()

public:
	ATileManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// All tiles currently on the board
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tiles")
	TArray<ATile*> RegisteredTiles;

	// All Residential Tiles currently on the board
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tiles")
	TArray<ATile*> ResidentialTiles;

	// All Forest Tiles currently on the board
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tiles")
	TArray<ATile*> ForestTiles;

	// All Grass Tiles currently on the board
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tiles")
	TArray<ATile*> GrassTiles;

	UPROPERTY(EditAnywhere, Category = "Tiles")
	TSubclassOf<ATile> TileClass;

	// Fast lookup by cube coordinate
	UPROPERTY()
	TMap<FIntVector, ATile*> TileLookup;

	// Community health from 0-100
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int32 CommunityHealth = 100;

	// Tile ID to assign when a tile becomes charred
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	int32 CharredTileID = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	int32 GrassTileID = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	int32 NonBurnableMountainTileID = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	int32 BurnableMountainTileID = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	int32 CommunicationsTowerTileID = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	int32 WaterTowerTileID = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	int32 FireStationTileID = 9;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	int32 CharredMountainTileID = 10;

	// Reference to GameManager (for WindDirection). If not set, TileManager will auto-find one at BeginPlay.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	AGameManager* GameManager = nullptr;

	// Chance for NON-wind neighbors to ignite: Numerator / Denominator
	// Currently 2/6 = 33% each of the 5 non-wind direction neighbors.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire|Spread")
	int32 NonWindSpreadNumerator = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire|Spread")
	int32 NonWindSpreadDenominator = 6;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	void RegisterTile(ATile* Tile);

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	void UnregisterTile(ATile* Tile);

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	ATile* FindSafeTileToBurn(int32 MinimumDistance);

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void UpdateFirePreview(); // Marks bWillIgniteNextTurn based on current burning tiles, without advancing fire.

	// Called once per "fire step" (end of player turn)
	// Now includes:
	//  - Ignite tiles planned from previous turn
	//  - Burn down current burning tiles with 0 health
	//  - Plan next-turn ignitions (pre-fire indicator)
	UFUNCTION(BlueprintCallable, Category = "Fire")
	void ExecuteFireTurn();

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void IgniteTile(ATile* Tile);

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void BurnOutTile(ATile* Tile);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Data")
	UDataTable* TileDataTable = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	TArray<ATile*> GetNeighborTiles(const FIntVector& Center) const;


protected:
	// Decrement fire health and handle burnout (NOTE: does NOT plan spread targets anymore)
	void ProcessBurningTile(ATile* BurningTile, TSet<ATile*>& OutTilesToIgnite);

	void ApplyCommunityDamage(ATile* Tile);

	// Routing function for tile-specific spread rules
	void GetSpreadTargetsForTile(ATile* SourceTile, TSet<ATile*>& OutTilesToIgnite) const;

	// Default rule
	void GetDefaultSpreadTargets(ATile* SourceTile, TSet<ATile*>& OutTilesToIgnite) const;

	// Tile-specified rule
	void GetGrassSpreadTargets(ATile* SourceTile, TSet<ATile*>& OutTilesToIgnite) const;

	// Helper: clear all bWillIgniteNextTurn flags (and refresh visuals if changed)
	void ClearPreFireIndicators();

	// Helper: get wind direction 0..5 from GameManager (or -1 if unavailable)
	int32 GetWindDirectionIndex() const;
};
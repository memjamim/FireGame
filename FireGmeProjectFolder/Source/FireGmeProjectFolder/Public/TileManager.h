#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileManager.generated.h"

class ATile;
class UDataTable;

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
	int32 MountainTileID = 2;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	void RegisterTile(ATile* Tile);

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	void UnregisterTile(ATile* Tile);

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void ExecuteFireTurn();

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void IgniteTile(ATile* Tile);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Data")
	UDataTable* TileDataTable = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	TArray<ATile*> GetNeighborTiles(const FIntVector& Center) const;


protected:
	void ProcessBurningTile(ATile* BurningTile, TSet<ATile*>& OutTilesToIgnite);
	void BurnOutTile(ATile* Tile);
	void ApplyCommunityDamage(ATile* Tile);

	// Routing function for tile-specific spread rules
	void GetSpreadTargetsForTile(ATile* SourceTile, TSet<ATile*>& OutTilesToIgnite) const;

	// Default rule
	void GetDefaultSpreadTargets(ATile* SourceTile, TSet<ATile*>& OutTilesToIgnite) const;

	// Tile-specified rule
	void GetGrassSpreadTargets(ATile* SourceTile, TSet<ATile*>& OutTilesToIgnite) const;
};
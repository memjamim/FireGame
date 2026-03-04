#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitDataRow.h"
#include "Unit.generated.h"

class ATile;
class ATileManager;

UCLASS()
class FIREGMEPROJECTFOLDER_API AUnit : public AActor
{
	GENERATED_BODY()

public:
	AUnit();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* UnitMesh;

	/** Cached reference to the TileManager in the world. Resolved in BeginPlay. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit|References")
	ATileManager* TileManager;

public:
	virtual void Tick(float DeltaTime) override;

	/** Assign in the editor — points to DT_Unit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Data")
	UDataTable* UnitDataTable = nullptr;

	/** Runtime copy of the row data for this unit instance. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit|Data")
	FUnitData UnitData;

	/** Read a row from UnitDataTable and apply its data to this unit. */
	UFUNCTION(BlueprintCallable, Category = "Unit|Data")
	bool ApplyDataFromRowName(FName RowName);

	/** Cube coordinates on the hex grid. Default spawn is (0,0,0). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Position")
	FIntVector GridCoordinates = FIntVector::ZeroValue;

	/** The tile this unit is currently standing on. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit|Position")
	ATile* CurrentTile = nullptr;

	/** Set the unit's current tile and update GridCoordinates to match. */
	UFUNCTION(BlueprintCallable, Category = "Unit|Position")
	void SetCurrentTile(ATile* NewTile);

	/** Return the tile the unit is standing on. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Unit|Position")
	ATile* GetCurrentTile() const { return CurrentTile; }

	/** Get the six neighbors of the unit's current tile via TileManager. */
	UFUNCTION(BlueprintCallable, Category = "Unit|Position")
	TArray<ATile*> GetAdjacentTiles() const;

	/** Move the unit to a target tile by cube coordinates. */
	UFUNCTION(BlueprintCallable, Category = "Unit|Movement")
	bool MoveToTile(FIntVector TargetCoordinates);

	/** Override in Blueprint for custom validation (range, obstacles, terrain). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit|Movement")
	bool CanMoveToTile(ATile* TargetTile);

	/** Called after a move completes. Override in Blueprint. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit|Movement")
	void OnMoveComplete();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Stamina")
	int32 CurrentStamina = 100;

	UFUNCTION(BlueprintCallable, Category = "Unit|Stamina")
	void ConsumeStamina(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Unit|Stamina")
	void RestoreStamina(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Unit|Stamina")
	bool HasEnoughStamina(int32 Required) const;

	UPROPERTY(BlueprintReadWrite, Category = "Unit|Selection")
	bool bIsSelected = false;

	UFUNCTION(BlueprintCallable, Category = "Unit|Selection")
	void Select();

	UFUNCTION(BlueprintCallable, Category = "Unit|Selection")
	void Deselect();

	/** Override in Blueprint for visual feedback. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit|Selection")
	void OnSelected();

	/** Override in Blueprint to remove visual feedback. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit|Selection")
	void OnDeselected();

	UPROPERTY(BlueprintReadWrite, Category = "Unit|Turn")
	bool bHasActedThisTurn = false;

	UFUNCTION(BlueprintCallable, Category = "Unit|Turn")
	void StartTurn();

	UFUNCTION(BlueprintCallable, Category = "Unit|Turn")
	void EndTurn();

	UFUNCTION(BlueprintCallable, Category = "Unit|Abilities")
	bool CanUseAbility(int32 AbilityIndex);

	/** Override in Blueprint per-unit-type (Helicopter, Residential FF, Wildland FF). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit|Abilities")
	void UseAbility(int32 AbilityIndex, ATile* TargetTile);

	/** Override in Blueprint to list ability names. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit|Abilities")
	TArray<FString> GetAvailableAbilities();

	UFUNCTION(BlueprintPure, Category = "Unit|Coordinates")
	static int32 GetCubeDistance(FIntVector CubeA, FIntVector CubeB);

	UFUNCTION(BlueprintPure, Category = "Unit|Coordinates")
	static bool IsValidCubeCoordinate(FIntVector Cube);

private:
	/** Finds the TileManager singleton in the world. */
	ATileManager* FindTileManager() const;
};
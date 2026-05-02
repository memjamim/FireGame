#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileDataRow.h"
#include "Engine/DataTable.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h" 
#include "Engine/Texture2D.h"       
#include "Tile.generated.h"

// Types of Tiles
UENUM(BlueprintType)
enum class TileIDs : uint8
{
	FOREST_ID = 0,
	GRASS_ID = 1,
	WATER_ID = 2,
	CHARRED_ID = 3,
	RESIDENTIAL_ID = 4,
	ROCKY_MOUNTAIN_ID = 5,
	GRASSY_MOUNTAIN_ID = 6,
	COMMUNICATIONS_TOWER_ID = 7,
	WATER_TOWER_ID = 8,
	FIRE_STATION_ID = 9,
	SCHOOL_ID = 11,
};

class UNiagaraSystem;
class ATileManager;
class UTextRenderComponent;
class UBillboardComponent;

UCLASS()
class FIREGMEPROJECTFOLDER_API ATile : public AActor
{
	GENERATED_BODY()

public:
	ATile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile|Alert")
	UBillboardComponent* AlertIndicator = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tile|Alert")
	TObjectPtr<UTexture2D> AlertIndicatorTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Alert")
	float AlertIndicatorZOffset = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Alert")
	float AlertTurnsTextPitchOffset = -90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Alert")
	bool bMirrorAlertTurnsText = false;

	UFUNCTION(BlueprintCallable, Category = "Tile|Alert")
	void SetAlertIndicatorVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Tile|Alert")
	bool IsAlertIndicatorVisible() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile|Alert")
	UTextRenderComponent* AlertTurnsText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Alert")
	int32 AlertTurnsRemaining = 0;

	UFUNCTION(BlueprintCallable, Category = "Tile|Alert")
	void SetAlertTurnsRemaining(int32 InTurnsRemaining);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Alert")
	bool bAlertTurnsTextFacesCamera = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Alert")
	float AlertTurnsTextYawOffset = 180.0f;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	void UpdateAlertTurnsTextFacingCamera() const;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	int32 TileID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	bool bIsBurnable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	bool bIsBurning = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Fire")
	bool bWillIgniteNextTurn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	int32 CurrentFireHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	int32 CommunityHealthCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	FIntVector GridCoordinates = FIntVector::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tile")
	UStaticMeshComponent* TileMesh;

	UPROPERTY(BlueprintReadOnly)
	UStaticMesh* CurrentMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	bool bHasBeenDestroyedByFire = false;

	// Static mesh

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Data")
	UDataTable* TileDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Data")
	FName TileRowName = NAME_None;

	// If true (default), keep GridCoordinates synced from actor world location during construction / updates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Grid")
	bool bAutoSyncGridCoordinates = true;

	UPROPERTY()
	ATileManager* TileManager;

	// NEW: Data-driven visuals info (BP can read these)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Visual")
	TArray<TObjectPtr<UStaticMesh>> TileModels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Visual")
	TObjectPtr<UMaterialInterface> BaseMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Visual")
	TObjectPtr<UMaterialInterface> PreFireMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile|Visual")
	TObjectPtr<UMaterialInterface> BurningMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tile|FX")
	TObjectPtr<UNiagaraSystem> ExtinguishEffect = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Tile")
	void Ignite();

	UFUNCTION(BlueprintCallable, Category = "Tile")
	void Extinguish();

	UFUNCTION(BlueprintImplementableEvent, Category = "Tile")
	void UpdateTileVisuals();

	UFUNCTION(BlueprintCallable, Category = "Tile|Data")
	bool ApplyDataFromID(int32 InTileID);

	UFUNCTION(BlueprintCallable, Category = "Tile|Data")
	bool ApplyDataFromRowName(FName RowName);

	UFUNCTION(BlueprintCallable, Category = "Tile|Data")
	void ReduceCommunityHealthCost();

	UFUNCTION(BlueprintCallable, Category = "Tile|Data")
	int32 GetTileID();

	// Called by C++; actual implementation is in BP_Tile
	UFUNCTION(BlueprintNativeEvent, Category = "Tile|Grid")
	FIntVector ConvertWorldToGridCoordinates(const FVector& WorldLocation) const;
	virtual FIntVector ConvertWorldToGridCoordinates_Implementation(const FVector& WorldLocation) const;

	// Optional reverse conversion for future procedural generation in C++
	UFUNCTION(BlueprintNativeEvent, Category = "Tile|Grid")
	FVector ConvertGridToWorldCoordinates(const FIntVector& InGridCoordinates) const;
	virtual FVector ConvertGridToWorldCoordinates_Implementation(const FIntVector& InGridCoordinates) const;

	UFUNCTION(BlueprintCallable, Category = "Tile|Grid")
	void SyncGridCoordinatesFromWorld();

	UFUNCTION(BlueprintCallable, Category = "Tile|Grid")
	void SyncWorldFromGrid();

	// Copied select-functionality from unit

	UPROPERTY(BlueprintReadWrite, Category = "Tile|Selection")
	bool bIsSelected = false;

	UFUNCTION(BlueprintCallable, Category = "Tile|Selection")
	void Select();

	UFUNCTION(BlueprintCallable, Category = "Tile|Selection")
	void Deselect();

	/** Override in Blueprint for visual feedback. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tile|Selection")
	void OnSelected();

	/** Override in Blueprint to remove visual feedback. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tile|Selection")
	void OnDeselected();

	/** Override in Blueprint for visual feedback. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tile|Selection")
	void ExtinguishTilesVisuals();
	virtual void ExtinguishTilesVisuals_Implementation();
};
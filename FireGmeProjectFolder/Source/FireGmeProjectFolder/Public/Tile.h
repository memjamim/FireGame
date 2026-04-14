#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileDataRow.h"
#include "Engine/DataTable.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h" 
#include "Engine/Texture2D.h"       
#include "Tile.generated.h"

class ATileManager;

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
	float AlertIndicatorZOffset = 300.0f;

	UFUNCTION(BlueprintCallable, Category = "Tile|Alert")
	void SetAlertIndicatorVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Tile|Alert")
	bool IsAlertIndicatorVisible() const;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

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
};
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileDataRow.h"
#include "Engine/DataTable.h"
#include "Tile.generated.h"

UCLASS()
class FIREGMEPROJECTFOLDER_API ATile : public AActor
{
	GENERATED_BODY()

public:
	ATile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	int32 TileID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	bool bIsBurnable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	bool bIsBurning = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	int32 CurrentFireHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	int32 CommunityHealthCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	FIntVector GridCoordinates = FIntVector::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	bool bHasBeenDestroyedByFire = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tile|Data")
	UDataTable* TileDataTable = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Tile")
	void Ignite();

	UFUNCTION(BlueprintCallable, Category = "Tile")
	void Extinguish();

	UFUNCTION(BlueprintImplementableEvent, Category = "Tile")
	void UpdateTileVisuals();

	UFUNCTION(BlueprintCallable, Category="Tile|Data")
	bool ApplyDataFromID(int32 InTileID);

	UFUNCTION(BlueprintCallable, Category="Tile|Data")
	bool ApplyDataFromRowName(FName RowName);
};
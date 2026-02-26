#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TileDataRow.generated.h"

USTRUCT(BlueprintType)
struct FIREGMEPROJECTFOLDER_API FTileDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Is_Burnable = true;

	// This should be treated as a default only
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Is_Burning = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Tile_Health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Community_Health_Cost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIntVector Coordinates = FIntVector::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* Model = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInterface* Material = nullptr;
};
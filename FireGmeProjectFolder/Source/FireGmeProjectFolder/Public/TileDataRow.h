#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TileDataRow.generated.h"

USTRUCT(BlueprintType)
struct FIREGMEPROJECTFOLDER_API FTileDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Is_Burnable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Is_Burning = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Movement_Cost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Tile_Health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Community_Health_Cost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector Coordinates = FIntVector::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Data")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UStaticMesh>> Models;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInterface> Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInterface> PreFireMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInterface> BurningMaterial = nullptr;


	// fallback if no meshes exist in table
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> Model = nullptr;
};
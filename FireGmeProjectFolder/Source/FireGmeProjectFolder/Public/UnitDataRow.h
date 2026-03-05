#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UnitDataRow.generated.h"

class AUnit;

/**
 * Data structure that matches DT_Unit.
 * Each row represents a unit type (Helicopter, Residential Firefighter, Wildland Firefighter, etc.).
 */
USTRUCT(BlueprintType)
struct FIREGMEPROJECTFOLDER_API FUnitData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
	int32 ID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
	int32 Unit_Movement_Type = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
	int32 Stamina = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
	FIntVector Coordinates = FIntVector::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
	TSoftObjectPtr<UStaticMesh> Model;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
	TSoftObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
	int32 Action_Cost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
	int32 Turns_To_Deploy = 0;

	/** Blueprint class to spawn for this unit type (Helicopter, Residential FF, Wildland FF). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Data")
	TSubclassOf<AUnit> Unit_BP;
};
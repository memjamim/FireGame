#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AlertDataRow.generated.h"

UENUM(BlueprintType)
enum class EAlertEffectType : uint8
{
	None = 0,
	AddActionPoints,
	RemoveActionPoints,
	AddCityHealth,
	RemoveCityHealth,
	ChangeWindDirection,
	SpawnFire,
	SpawnUnit,
	Custom
};

USTRUCT(BlueprintType)
struct FIREGMEPROJECTFOLDER_API FAlertUnitRequirement
{
	GENERATED_BODY()

	// Unit row name from DT_Unit (or other identifier you use)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirement")
	int32 RequiredUnitID = 0;

	// Required distance from event tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirement")
	int32 RequiredRadius = 0;
};

USTRUCT(BlueprintType)
struct FIREGMEPROJECTFOLDER_API FAlertOptionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
	FText OptionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
	int32 ActionPointCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
	FAlertUnitRequirement UnitRequirement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
	EAlertEffectType EffectType = EAlertEffectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
	int32 EffectMagnitude = 0;

	// Optional custom payload for Blueprint or C++ dispatch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
	FName EffectPayload = NAME_None;
};

USTRUCT(BlueprintType)
struct FIREGMEPROJECTFOLDER_API FAlertData : public FTableRowBase
{
	GENERATED_BODY()

	// Stable identifier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	FName AlertId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	FText Narrative;

	// If empty, can spawn on any tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	TArray<FName> AllowedTileTypes;

	// How many turns player has to respond
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	int32 TurnsToRespond = 2;

	// Spawn weighting for random selection
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	int32 Weight = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	TArray<FAlertOptionData> Options;
};
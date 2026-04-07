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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirement")
	int32 RequiredUnitID = 0;

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

	// Existing requirement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option|Requirement")
	FAlertUnitRequirement UnitRequirement;

	// New optional requirements
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option|Requirement")
	int32 MinimumCityHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option|Requirement")
	int32 MaximumCityHealth = MAX_int32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option|Requirement")
	int32 MinimumTurn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option|Requirement")
	int32 MaximumTurn = MAX_int32;

	// -1 = any direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option|Requirement")
	int32 RequiredWindDirection = -1;

	// Optional UI message when blocked
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option|Requirement")
	FText UnavailableReasonOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
	EAlertEffectType EffectType = EAlertEffectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
	int32 EffectMagnitude = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
	FName EffectPayload = NAME_None;
};

USTRUCT(BlueprintType)
struct FIREGMEPROJECTFOLDER_API FAlertData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	FName AlertId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	FText Narrative;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert|UI")
	TSoftObjectPtr<UTexture2D> SplashImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	TArray<FName> AllowedTileTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	int32 TurnsToRespond = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	int32 Weight = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	TArray<FAlertOptionData> Options;
};
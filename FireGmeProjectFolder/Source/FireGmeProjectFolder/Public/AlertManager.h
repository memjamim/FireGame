#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "AlertDataRow.h"
#include "AlertManager.generated.h"

class AGameManager;
class ATileManager;
class AAudioManager;
class ATile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlertSpawned, int32, AlertInstanceId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlertResolved, int32, AlertInstanceId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlertExpired, int32, AlertInstanceId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlertSelected, int32, AlertInstanceId);

USTRUCT(BlueprintType)
struct FIREGMEPROJECTFOLDER_API FActiveAlertInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	int32 InstanceId = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	FName AlertRowName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	FIntVector TileCoords = FIntVector::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	int32 SpawnTurn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	int32 TurnsRemaining = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	bool bResolved = false;
};

USTRUCT(BlueprintType)
struct FIREGMEPROJECTFOLDER_API FAlertOptionAvailability
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	bool bIsAvailable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
	FText BlockReason;
};

UCLASS()
class FIREGMEPROJECTFOLDER_API AAlertManager : public AActor
{
	GENERATED_BODY()

public:
	AAlertManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alert|Data")
	UDataTable* AlertDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert|Rules")
	int32 MaxConcurrentAlerts = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert|Rules", meta = (ClampMin = "0", ClampMax = "100"))
	int32 SpawnChancePercent = 45;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert|Rules", meta = (ClampMin = "0"))
	int32 GuaranteedSpawnEveryNTurns = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert|Rules", meta = (ClampMin = "0"))
	int32 ExpiredAlertCityHealthPenalty = 10;

	UPROPERTY(BlueprintReadOnly, Category = "Alert|Runtime")
	TArray<FActiveAlertInstance> ActiveAlerts;

	UPROPERTY(BlueprintAssignable, Category = "Alert|Events")
	FOnAlertSpawned OnAlertSpawned;

	UPROPERTY(BlueprintAssignable, Category = "Alert|Events")
	FOnAlertResolved OnAlertResolved;

	UPROPERTY(BlueprintAssignable, Category = "Alert|Events")
	FOnAlertExpired OnAlertExpired;

	UPROPERTY(BlueprintAssignable, Category = "Alert|Events")
	FOnAlertSelected OnAlertSelected;

	UFUNCTION(BlueprintCallable, Category = "Alert")
	bool TrySpawnRandomAlert();

	UFUNCTION(BlueprintCallable, Category = "Alert")
	bool ResolveAlertOption(int32 AlertInstanceId, int32 OptionIndex);

	UFUNCTION(BlueprintCallable, Category = "Alert")
	void ProcessTurnStart();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Alert")
	bool GetAlertDisplayData(int32 AlertInstanceId, FActiveAlertInstance& OutInstance, FAlertData& OutData) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Alert")
	bool GetActiveAlertById(int32 AlertInstanceId, FActiveAlertInstance& OutAlertInstance) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Alert")
	bool GetAlertOptionAvailability(int32 AlertInstanceId, TArray<FAlertOptionAvailability>& OutAvailability) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Alert")
	bool CanResolveAlertOption(int32 AlertInstanceId, int32 OptionIndex, FText& OutFailureReason) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Alert")
	const TArray<FActiveAlertInstance>& GetActiveAlerts() const { return ActiveAlerts; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Alert")
	bool HasUnresolvedAlerts() const;

protected:
	UPROPERTY()
	AGameManager* GameManager = nullptr;

	UPROPERTY()
	ATileManager* TileManager = nullptr;

	UPROPERTY()
	AAudioManager* AudioManager = nullptr;

	UPROPERTY()
	int32 NextAlertInstanceId = 1;

	UPROPERTY()
	int32 LastSpawnTurn = 0;

	UPROPERTY()
	TMap<int32, FIntVector> AlertIdToTileCoords;

	UPROPERTY()
	TMap<FIntVector, int32> TileCoordsToAlertId;

	UPROPERTY()
	bool bWasLeftMouseDownLastTick = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "Alert")
	void OnAlertSpawned_BP(const FActiveAlertInstance& AlertInstance, const FAlertData& AlertData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Alert")
	void OnAlertResolved_BP(const FActiveAlertInstance& AlertInstance, int32 OptionIndex, const FAlertOptionData& OptionData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Alert")
	void OnAlertExpired_BP(const FActiveAlertInstance& AlertInstance, const FAlertData& AlertData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Alert")
	void OnAlertCustomEffect_BP(const FActiveAlertInstance& AlertInstance, const FAlertOptionData& OptionData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Alert")
	void OnAlertSelected_BP(int32 AlertInstanceId);

private:
	bool CacheManagerReferences();
	bool IsTileAllowedForAlert(const ATile* Tile, const FAlertData& AlertData) const;
	bool DoesUnitRequirementPass(const FIntVector& AlertTileCoords, const FAlertUnitRequirement& Requirement) const;
	bool EvaluateOptionRequirements(const FActiveAlertInstance& Instance, const FAlertOptionData& OptionData, FText& OutFailureReason) const;

	// Spawn condition helpers
	FName GetEffectiveAlertId(const FName& RowName, const FAlertData& AlertData) const;
	bool DoesAlertMeetSpawnConditions(const FName& RowName, const FAlertData& AlertData) const;

	bool ApplyOptionEffect(const FActiveAlertInstance& Instance, const FAlertOptionData& OptionData);
	bool ApplyTemporaryActionPointIncomeEffect(const FAlertOptionData& OptionData);
	bool TryApplyActionPointCost(int32 ActionPointCost);

	int32 FindActiveAlertIndexById(int32 AlertInstanceId) const;
	void RemoveAlertAtIndex(int32 Index);

	void RegisterAlertTileLink(int32 AlertInstanceId, const FIntVector& TileCoords);
	void UnregisterAlertTileLink(int32 AlertInstanceId);
	void HandleAlertClickFromCursor();
	void SetTileAlertIndicator(const FIntVector& TileCoords, bool bVisible, int32 TurnsRemaining = 0);

	// Runtime history
	UPROPERTY()
	TSet<FName> SpawnedAlertIds;

	UPROPERTY()
	TSet<FName> ResolvedAlertIds;

};
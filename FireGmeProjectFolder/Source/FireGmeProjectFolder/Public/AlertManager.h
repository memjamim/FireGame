#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "AlertDataRow.h"
#include "AlertManager.generated.h"

class AGameManager;
class ATileManager;
class ATile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlertSpawned, int32, AlertInstanceId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlertResolved, int32, AlertInstanceId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlertExpired, int32, AlertInstanceId);

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

	UPROPERTY(BlueprintReadOnly, Category = "Alert|Runtime")
	TArray<FActiveAlertInstance> ActiveAlerts;

	UPROPERTY(BlueprintAssignable, Category = "Alert|Events")
	FOnAlertSpawned OnAlertSpawned;

	UPROPERTY(BlueprintAssignable, Category = "Alert|Events")
	FOnAlertResolved OnAlertResolved;

	UPROPERTY(BlueprintAssignable, Category = "Alert|Events")
	FOnAlertExpired OnAlertExpired;

	UFUNCTION(BlueprintCallable, Category = "Alert")
	bool TrySpawnRandomAlert();

	UFUNCTION(BlueprintCallable, Category = "Alert")
	bool ResolveAlertOption(int32 AlertInstanceId, int32 OptionIndex);

	UFUNCTION(BlueprintCallable, Category = "Alert")
	void ProcessTurnStart();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Alert")
	bool GetAlertDisplayData(int32 AlertInstanceId, FActiveAlertInstance& OutInstance, FAlertData& OutData) const;

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
	int32 NextAlertInstanceId = 1;

	UFUNCTION(BlueprintImplementableEvent, Category = "Alert")
	void OnAlertSpawned_BP(const FActiveAlertInstance& AlertInstance, const FAlertData& AlertData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Alert")
	void OnAlertResolved_BP(const FActiveAlertInstance& AlertInstance, int32 OptionIndex, const FAlertOptionData& OptionData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Alert")
	void OnAlertExpired_BP(const FActiveAlertInstance& AlertInstance, const FAlertData& AlertData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Alert")
	void OnAlertCustomEffect_BP(const FActiveAlertInstance& AlertInstance, const FAlertOptionData& OptionData);

private:
	bool CacheManagerReferences();
	bool IsTileAllowedForAlert(const ATile* Tile, const FAlertData& AlertData) const;
	bool DoesUnitRequirementPass(const FIntVector& AlertTileCoords, const FAlertUnitRequirement& Requirement) const;

	bool ApplyOptionEffect(const FActiveAlertInstance& Instance, const FAlertOptionData& OptionData);
	bool TryApplyActionPointCost(int32 ActionPointCost);

	int32 FindActiveAlertIndexById(int32 AlertInstanceId) const;
	void RemoveAlertAtIndex(int32 Index);
};
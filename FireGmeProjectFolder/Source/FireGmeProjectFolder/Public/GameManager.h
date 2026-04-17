#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "AudioManager.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "GameManager.generated.h"

class ATileManager;
class AUnit;
class AAlertManager;

// Turn State
UENUM(BlueprintType)
enum class TBGameState : uint8
{
	PLAYER_TURN = 0,
	UNIT_ACTING = 1,
	FIRE_TURN = 2,
	RANDOM_EVENTS = 3
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionPointsChanged, int32, NewActionPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnStarted, int32, NewTurnNumber);

// Queue delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeploymentQueueChanged);

/**
 * A single pending deployment entry
 */
USTRUCT(BlueprintType)
struct FIREGMEPROJECTFOLDER_API FPendingUnitDeployment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deployment")
	FName UnitRowName = NAME_None;

	// Where the unit should deploy (hex cube coords)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deployment")
	FIntVector SpawnCoords = FIntVector::ZeroValue;

	// Counts down each StartPlayerTurn(); deploys when <= 0
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deployment")
	int32 TurnsRemaining = 0;

	// Helpful for sorting/debugging/UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deployment")
	int32 TurnQueued = 0;
};

UCLASS()
class FIREGMEPROJECTFOLDER_API AGameManager : public AActor
{
	GENERATED_BODY()

public:
	AGameManager();

protected:
	virtual void BeginPlay() override;

	// Reference to tile manager
	UPROPERTY()
	ATileManager* TileManager = nullptr;

	AAlertManager* AlertManager = nullptr;
	void CacheAlertManager();

	// Reference to audio manager
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managers")
	AAudioManager* AudioManager = nullptr;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "GameManager", meta = (WorldContext = "WorldContextObject"))
	static AGameManager* GetGameManager(const UObject* WorldContextObject);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	int32 CityHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
	int32 WindDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	int32 ActionPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	TBGameState CurrentState;

	// Turn counter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	int32 CurrentTurn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Units")
	UDataTable* UnitDataTable = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnActionPointsChanged OnActionPointsChanged;

	// An array of all Units on the map at a given time.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Units")
	TArray<AUnit*> UnitsInPlay;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTurnStarted OnTurnStarted;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeploymentQueueChanged OnDeploymentQueueChanged;

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void SetActionPoints(int32 NewActionPoints);

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void AddActionPoints(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void ApplyActionPointsPerTurnModifier(int32 ModifierDelta, int32 DurationTurns);

	UFUNCTION(BlueprintCallable, Category = "Turns")
	bool TrySpendActionPoints(int32 Cost);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turns")
	int32 ActionPointsPerTurnModifier = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turns")
	int32 ActionPointsPerTurnModifierTurnsRemaining = 0;

	UFUNCTION(BlueprintCallable, Category = "Wind")
	void SetWindDirection(int32 NewWindDirection);

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void AdvanceTurnCounter();

	// ---- Turn flow
	UFUNCTION(BlueprintCallable, Category = "Turns")
	void EndTurn();

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void DoFireTurn();

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void StartPlayerTurn();

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void EndPlayerTurn();

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void DoRandomEvent();

	UFUNCTION(BlueprintCallable, Category = "Units")
	void RegisterUnit(AUnit* UnitToRegister);

	UFUNCTION(BlueprintCallable, Category = "Units")
	void UnregisterUnit(AUnit* UnitToDeregister);

	// Blueprint-readable so UI can show arrives in X turns
	UPROPERTY(BlueprintReadOnly, Category = "Deployments")
	TArray<FPendingUnitDeployment> PendingDeployments;

	/**
	 * UI calls this instead of spawning immediately.
	 * Spends AP immediately (Action_Cost)
	 * Adds a pending deployment with Turns_To_Deploy
	 */
	UFUNCTION(BlueprintCallable, Category = "Deployments")
	bool PurchaseAndQueueUnit(FName UnitRowName, FIntVector SpawnCoords);

	/** Allow UI to clear queue (debug) */
	UFUNCTION(BlueprintCallable, Category = "Deployments")
	void ClearDeploymentQueue();

	/** Helper for UI */
	UFUNCTION(BlueprintCallable, Category = "Deployments")
	const TArray<FPendingUnitDeployment>& GetPendingDeployments() const { return PendingDeployments; }

protected:
	// Tick down queue and deploy ready units at start of player turn
	void ProcessDeploymentQueue();

	// Spawn & initialize a unit
	AUnit* DeployUnitNow(const FPendingUnitDeployment& Deployment);

	/**
	 * Blueprint hook called when a unit is queued
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Deployments")
	void OnUnitQueued_BP(FPendingUnitDeployment Deployment);

	/**
	 * Blueprint hook called after a unit is deployed.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Deployments")
	void OnUnitDeployed_BP(AUnit* NewUnit, FName UnitRowName, FIntVector SpawnCoords);
};
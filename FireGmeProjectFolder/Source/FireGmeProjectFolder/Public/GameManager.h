#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "GameManager.generated.h"

class ATileManager;
class AUnit;

// Turn State
UENUM(BlueprintType)
enum class TBGameState : uint8
{
	PLAYER_TURN = 0,
	UNIT_ACTING = 1,
	FIRE_TURN = 2,
	RANDOM_EVENTS = 3
};

// Existing delegates you’re already broadcasting in .cpp :contentReference[oaicite:3]{index=3}
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionPointsChanged, int32, NewActionPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnStarted, int32, NewTurnNumber);

// Queue delegates (optional but useful for UI refresh)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeploymentQueueChanged);

/**
 * A single pending deployment entry (Blueprint-visible so UI can show turns remaining)
 */
USTRUCT(BlueprintType)
struct FIREGMEPROJECTFOLDER_API FPendingUnitDeployment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deployment")
	FName UnitRowName = NAME_None;

	// Where the unit should deploy (your hex cube coords)
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

public:
	virtual void Tick(float DeltaTime) override;

	// ---- Singleton-ish helper (you already have this in cpp) :contentReference[oaicite:4]{index=4}
	UFUNCTION(BlueprintCallable, Category = "GameManager", meta = (WorldContext = "WorldContextObject"))
	static AGameManager* GetGameManager(const UObject* WorldContextObject);

	// ---- Core game variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	int32 CityHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
	int32 WindDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	int32 ActionPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	TBGameState CurrentState;

	// Turn counter (your cpp uses CurrentTurn already) :contentReference[oaicite:5]{index=5}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	int32 CurrentTurn;

	// ---- Data
	// Set this on your BP_GameManager / MyGameManager instance in the editor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Units")
	UDataTable* UnitDataTable = nullptr;

	// ---- Events
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnActionPointsChanged OnActionPointsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTurnStarted OnTurnStarted;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeploymentQueueChanged OnDeploymentQueueChanged;

	// ---- AP helpers (you already have these in cpp) :contentReference[oaicite:6]{index=6}
	UFUNCTION(BlueprintCallable, Category = "Turns")
	void SetActionPoints(int32 NewActionPoints);

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void AddActionPoints(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Turns")
	bool TrySpendActionPoints(int32 Cost);

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

	// =========================================================
	//                 DEPLOYMENT QUEUE (NEW)
	// =========================================================

	// Blueprint-readable so your UI can show “arrives in X turns”
	UPROPERTY(BlueprintReadOnly, Category = "Deployments")
	TArray<FPendingUnitDeployment> PendingDeployments;

	/**
	 * UI calls this instead of spawning immediately.
	 * - Spends AP immediately (Action_Cost)
	 * - Adds a pending deployment with Turns_To_Deploy
	 */
	UFUNCTION(BlueprintCallable, Category = "Deployments")
	bool PurchaseAndQueueUnit(FName UnitRowName, FIntVector SpawnCoords);

	/** Optional: allow UI to clear queue (debug) */
	UFUNCTION(BlueprintCallable, Category = "Deployments")
	void ClearDeploymentQueue();

	/** Optional: helper for UI */
	UFUNCTION(BlueprintCallable, Category = "Deployments")
	const TArray<FPendingUnitDeployment>& GetPendingDeployments() const { return PendingDeployments; }

protected:
	// Tick down queue and deploy ready units at start of player turn
	void ProcessDeploymentQueue();

	// Spawn + initialize a unit
	AUnit* DeployUnitNow(const FPendingUnitDeployment& Deployment);

	/**
	 * Blueprint hook called when a unit is queued
	 * (great place to refresh shop UI, play SFX, etc.)
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Deployments")
	void OnUnitQueued_BP(const FPendingUnitDeployment& Deployment);

	/**
	 * Blueprint hook called after a unit is deployed (spawned + applied row + snapped to tile).
	 * This is where you can:
	 * - Get PlayerController 0 -> Cast BP_CameraController -> Add to SpawnedUnits array
	 * - Print confirmation strings
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Deployments")
	void OnUnitDeployed_BP(AUnit* NewUnit, FName UnitRowName, FIntVector SpawnCoords);
};
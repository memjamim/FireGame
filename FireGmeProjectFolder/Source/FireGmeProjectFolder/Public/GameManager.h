#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

// Turn State
UENUM(BlueprintType)
enum class TBGameState : uint8
{
	PLAYER_TURN = 0,
	UNIT_ACTING = 1,
	FIRE_TURN = 2,
	RANDOM_EVENTS = 3
};

class ATileManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnStarted, int32, NewTurn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionPointsChanged, int32, NewActionPoints);

UCLASS()
class FIREGMEPROJECTFOLDER_API AGameManager : public AActor
{
	GENERATED_BODY()

public:
	AGameManager();

protected:
	virtual void BeginPlay() override;

	// Reference to tile manager
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managers")
	ATileManager* TileManager = nullptr;

public:
	virtual void Tick(float DeltaTime) override;

	// =========================
	// Turn / Game State
	// =========================

	// City health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	int32 CityHealth;

	// Wind direction (0..5)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
	int32 WindDirection;

	// Action Points
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	int32 ActionPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	TBGameState CurrentState;

	// Current turn counter used for unit deployment math
	// Starts at 1 when the first player turn begins.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turns")
	int32 CurrentTurn = 0;

	// Events (for UI)
	UPROPERTY(BlueprintAssignable, Category = "Turns|Events")
	FOnTurnStarted OnTurnStarted;

	UPROPERTY(BlueprintAssignable, Category = "Turns|Events")
	FOnActionPointsChanged OnActionPointsChanged;


	// Find the active GameManager in the world
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game", meta = (WorldContext = "WorldContextObject"))
	static AGameManager* GetGameManager(const UObject* WorldContextObject);

	// Turn / AP getters & setters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Turns")
	int32 GetCurrentTurn() const { return CurrentTurn; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Turns")
	int32 GetActionPoints() const { return ActionPoints; }

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void SetActionPoints(int32 NewActionPoints);

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void AddActionPoints(int32 Delta);

	// Spend AP safely in BP
	UFUNCTION(BlueprintCallable, Category = "Turns")
	bool TrySpendActionPoints(int32 Cost);

	// Wind accessors
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Wind")
	int32 GetWindDirection() const { return WindDirection; }

	UFUNCTION(BlueprintCallable, Category = "Wind")
	void SetWindDirection(int32 NewWindDirection);

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

protected:
	// Increments the turn counter and notifies BP
	void AdvanceTurnCounter();
};
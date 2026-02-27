#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

// Turn State
UENUM(BlueprintType)
enum class TBGameState : uint8 {
	PLAYER_TURN = 0,
	UNIT_ACTING = 1,
	FIRE_TURN = 2,
	RANDOM_EVENTS = 3
};

class ATileManager;

UCLASS()
class FIREGMEPROJECTFOLDER_API AGameManager : public AActor
{
	GENERATED_BODY()

public:
	AGameManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// CIty health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	int32 CityHealth;

	// Action Points
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	int32 ActionPoints;

	// Wind direction
	UPROPERTY(EditAnywhere)
	FVector2D wind_direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	TBGameState CurrentState;

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void DoFireTurn();

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void StartPlayerTurn();

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void DoRandomEvent();
};
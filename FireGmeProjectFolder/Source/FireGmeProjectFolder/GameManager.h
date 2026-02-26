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

	UFUNCTION(BlueprintCallable)
	void EndTurn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Managers")
	ATileManager* TileManager = nullptr;

	UPROPERTY(EditAnywhere)
	uint32 ActionPoints;

	UPROPERTY(EditAnywhere)
	FVector2D wind_direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turns")
	TBGameState CurrentState;

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void DoFireTurn();

	UFUNCTION(BlueprintCallable, Category = "Turns")
	void DoRandomEvent();
};
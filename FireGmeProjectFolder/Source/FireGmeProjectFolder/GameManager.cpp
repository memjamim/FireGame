// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "TileManager.h"
#include "Kismet/GameplayStatics.h"

/** The maximum city health */
const int32 MAX_CITY_HEALTH = 100;

/** AP the player starts with */
const int32 STARTING_AP = 3;

/** The max AP a player can have to earn interest (9 is the max, so they can earn 3 total additional AP) */
const int32 INTEREST_CAP = 9;

/** Interest will be earned at multiples of this number (e.x. Balatro is 5) */
const int32 INTEREST_RATE = 3;

/** How much AP the player will receive at the start of their turn */
const int32 AP_PER_ROUND = 1;

// Sets default values
AGameManager::AGameManager()
{
	CityHealth = MAX_CITY_HEALTH;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// TODO: Probably need to decide this value
	ActionPoints = STARTING_AP;
	
	CurrentState = TBGameState::PLAYER_TURN;
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();

	if (!TileManager)
	{
		TArray<AActor*> FoundManagers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATileManager::StaticClass(), FoundManagers);

		if (FoundManagers.Num() > 0)
		{
			TileManager = Cast<ATileManager>(FoundManagers[0]);
		}
	}
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Handles State Transitions and Behavior at the End of a Turn
void AGameManager::EndTurn()
{
	UE_LOG(LogTemp, Log, TEXT("Ending Turn: %d"), static_cast<uint8>(CurrentState));
	switch (CurrentState)
	{
	case TBGameState::PLAYER_TURN:
		StartPlayerTurn();
		CurrentState = TBGameState::FIRE_TURN;
		DoFireTurn();
		break;
	case TBGameState::FIRE_TURN:
		CurrentState = (rand() % 100) > 90 ? TBGameState::RANDOM_EVENTS : TBGameState::PLAYER_TURN;
		DoRandomEvent();
		break;
	case TBGameState::RANDOM_EVENTS:
		CurrentState = TBGameState::PLAYER_TURN;
		break;
	case TBGameState::UNIT_ACTING:
		CurrentState = TBGameState::PLAYER_TURN;
		break;
	};
}

void AGameManager::StartPlayerTurn()
{
	int LastStandBonus;

	if (CityHealth <= 30 && CityHealth > 20) {
		LastStandBonus = 1;
	}
	else if (CityHealth <= 20 && CityHealth > 10) {
		LastStandBonus = 2;
	}
	else if (CityHealth <= 10 && CityHealth > 0) {
		LastStandBonus = 3;
	}
	else {
		LastStandBonus = 0;
	}

	int EligibleInterestAP = FMath::Min(ActionPoints, INTEREST_CAP);
	int InterestBonus = (EligibleInterestAP / INTEREST_RATE);
	int AdditionalAP = (AP_PER_ROUND + InterestBonus + LastStandBonus);
	ActionPoints += AdditionalAP;

	UE_LOG(LogTemp, Log, TEXT("Player turn started. Gained %d AP (Base: %d, Interest: %d, Last Stand: %d). Total AP: %d"), AdditionalAP, AP_PER_ROUND, InterestBonus, LastStandBonus, ActionPoints);

}

// Handles all fire updates this turn.
void AGameManager::DoFireTurn()
{
	if (TileManager)
	{
		TileManager->ExecuteFireTurn();
	}

	// After fire resolves, return to player turn for now
	CurrentState = TBGameState::PLAYER_TURN;
}

// Does random events
void AGameManager::DoRandomEvent()
{
	// First should probably handle the occassional shift in winds
	// Then should have a small chance to trigger random events
	return;
}
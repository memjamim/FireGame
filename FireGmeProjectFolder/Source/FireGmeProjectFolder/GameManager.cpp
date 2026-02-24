// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// TODO: Probably need to decide this value
	ActionPoints = 100;
	CurrentState = TBGameState::PLAYER_TURN;
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Handles State Transitions and Behavior at the End of a Turn
void AGameManager::EndTurn()
{
	UE_LOG(LogTemp, Log, TEXT("Ending Turn: %d"), CurrentState);
	switch (CurrentState)
	{
	case TBGameState::PLAYER_TURN:
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

// Handles all fire updates this turn.
void AGameManager::DoFireTurn()
{
	// Should probably talk to/wait for some FireManager class to do this
	return;
}

// Does random events
void AGameManager::DoRandomEvent()
{
	// First should probably handle the occassional shift in winds
	// Then should have a small chance to trigger random events
	return;
}
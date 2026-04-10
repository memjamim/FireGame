// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManager.h"
#include "TileManager.h"
#include "Engine/Engine.h"
#include "Unit.h"
#include "Tile.h"
#include "AudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "UnitDataRow.h"

/** The maximum city health */
static const int32 MAX_CITY_HEALTH = 10;

/** AP the player starts with */
static const int32 STARTING_AP = 3;

/** The max AP a player can have to earn interest (9 is the max, so they can earn 3 total additional AP) */
static const int32 INTEREST_CAP = 9;

/** Interest will be earned at multiples of this number (e.x. Balatro is 5) */
static const int32 INTEREST_RATE = 3;

/** How much AP the player will receive at the start of their turn */
static const int32 AP_PER_ROUND = 1;

// Sets default values
AGameManager::AGameManager()
{
	CityHealth = MAX_CITY_HEALTH;

	PrimaryActorTick.bCanEverTick = true;

	ActionPoints = STARTING_AP;
	CurrentState = TBGameState::PLAYER_TURN;

	WindDirection = rand() % 6;

	// Turn counter starts at 0 and becomes 1 on the first StartPlayerTurn().
	CurrentTurn = 0;
}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();

	// Find TileManager and AudioManager if not assigned
	if (!TileManager)
	{
		TArray<AActor*> FoundTileManagers;
		TArray<AActor*> FoundAudioManagers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATileManager::StaticClass(), FoundTileManagers);
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAudioManager::StaticClass(), FoundAudioManagers);

		if (FoundTileManagers.Num() > 0)
		{
			TileManager = Cast<ATileManager>(FoundTileManagers[0]);
		}
		if (FoundAudioManagers.Num() > 0)
		{
			AudioManager = Cast<AAudioManager>(FoundAudioManagers[0]);
		}
	}

	// Optionally start turn 1 immediately (so UI can show "Turn 1" at game start)
	StartPlayerTurn();
}

void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AGameManager* AGameManager::GetGameManager(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(World, AGameManager::StaticClass(), Found);
	return (Found.Num() > 0) ? Cast<AGameManager>(Found[0]) : nullptr;
}

void AGameManager::SetActionPoints(int32 NewActionPoints)
{
	ActionPoints = NewActionPoints;
	OnActionPointsChanged.Broadcast(ActionPoints);
}

void AGameManager::AddActionPoints(int32 Delta)
{
	ActionPoints += Delta;
	OnActionPointsChanged.Broadcast(ActionPoints);
}

bool AGameManager::TrySpendActionPoints(int32 Cost)
{
	if (Cost <= 0)
	{
		return true;
	}

	if (ActionPoints < Cost)
	{
		return false;
	}

	ActionPoints -= Cost;
	OnActionPointsChanged.Broadcast(ActionPoints);
	return true;
}

void AGameManager::SetWindDirection(int32 NewWindDirection)
{
	// Keep it 0..5
	WindDirection = ((NewWindDirection % 6) + 6) % 6;
}

void AGameManager::AdvanceTurnCounter()
{
	CurrentTurn += 1;
	OnTurnStarted.Broadcast(CurrentTurn);
}

// Handles State Transitions and Behavior at the End of a Turn
void AGameManager::EndTurn()
{
	UE_LOG(LogTemp, Log, TEXT("Ending Turn: %d"), static_cast<uint8>(CurrentState));

	// Upon the ending of a turn (i.e., when the End Turn button is clicked), play the End Turn button sound.
	// This calls the Singleton of rhe Audio Manager in order to call the function found within it.
	/*if (AAudioManager* AM = AAudioManager::Get(GetWorld()))
	{
		AM->PlayEndTurnButtonSound();
	}*/
	switch (CurrentState)
	{
	case TBGameState::PLAYER_TURN:
		// End player actions -> proceed to fire turn
		UE_LOG(LogTemp, Log, TEXT("UnitsInPlay array size: %d"), UnitsInPlay.Num());
		for (AUnit* Unit : UnitsInPlay)
		{
			if (!Unit || !Unit->CurrentTile) // Check if both the Unit and the Tile are valid.
				continue;

			if (Unit->UnitData.ID != 2) // Check if the Unit is a Residential Firefiter.
				continue;

			if (Unit->CurrentTile->TileID != 4) // Check if the Unit's CurrentTile is a Residential Tile.
				continue;

			UE_LOG(LogTemp, Log, TEXT("Residential Firefighter has reduced health cost of a tile."));

			Unit->EvacuateResidents();
		}
		AudioManager->PlayEndTurnButtonSound();
		CurrentState = TBGameState::FIRE_TURN;
		DoFireTurn();
		break;

	case TBGameState::FIRE_TURN:
		CurrentState = TBGameState::RANDOM_EVENTS;
		DoRandomEvent();
		break;

	case TBGameState::RANDOM_EVENTS:
		// Random events done -> next player turn begins
		CurrentState = TBGameState::PLAYER_TURN;
		StartPlayerTurn();
		break;

	case TBGameState::UNIT_ACTING:
		CurrentState = TBGameState::PLAYER_TURN;
		break;
	}
}

void AGameManager::StartPlayerTurn()
{
	// Advance the turn counter at the start of the player turn.
	AdvanceTurnCounter();

	int LastStandBonus;

	if (CityHealth <= 30 && CityHealth > 20)
	{
		LastStandBonus = 1;
	}
	else if (CityHealth <= 20 && CityHealth > 10)
	{
		LastStandBonus = 2;
	}
	else if (CityHealth <= 10 && CityHealth > 0)
	{
		LastStandBonus = 3;
	}
	else
	{
		LastStandBonus = 0;
	}

	const int EligibleInterestAP = FMath::Min(ActionPoints, INTEREST_CAP);
	const int InterestBonus = (EligibleInterestAP / INTEREST_RATE);
	const int AdditionalAP = (AP_PER_ROUND + InterestBonus + LastStandBonus);

	ActionPoints += AdditionalAP;
	OnActionPointsChanged.Broadcast(ActionPoints);

	// REFRESH UNITS
	TArray<AActor*> FoundUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), FoundUnits);

	for (AActor* Actor : FoundUnits)
	{
		if (AUnit* Unit = Cast<AUnit>(Actor))
		{
			Unit->StartTurn();
		}
	}

	// PROCESS DEPLOYMENT QUEUE
	ProcessDeploymentQueue();

	UE_LOG(LogTemp, Log, TEXT("Player turn %d started. Gained %d AP (Base: %d, Interest: %d, Last Stand: %d). Total AP: %d"),
		CurrentTurn, AdditionalAP, AP_PER_ROUND, InterestBonus, LastStandBonus, ActionPoints);
}

void AGameManager::EndPlayerTurn()
{
	TArray<AActor*> FoundUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), FoundUnits);

	for (AActor* Actor : FoundUnits)
	{
		if (AUnit* Unit = Cast<AUnit>(Actor))
		{
			Unit->EndTurn();
		}
	}
}

// Handles all fire updates this turn.
void AGameManager::DoFireTurn()
{
	EndPlayerTurn();

	if (TileManager)
	{
		TileManager->ExecuteFireTurn();
	}

	// After fire resolves, end turn -> random events
	EndTurn();
}

// Does random events
void AGameManager::DoRandomEvent()
{
	// Occasional shift in winds
	if (rand() % 2 == 0)
	{
		const int32 Temp = WindDirection;
		while (WindDirection == Temp)
		{
			WindDirection = rand() % 6;
		}
	}

	// Random events done -> next player turn
	EndTurn();
}

// Adds a new Unit to the GameManager's array.
void AGameManager::RegisterUnit(AUnit* UnitToRegister)
{
	if (UnitToRegister)
	{
		UnitsInPlay.AddUnique(UnitToRegister);
	}
}

// Removes a Unit from the GameManager's array (this likely won't be used, but it may be nice to have).
void AGameManager::UnregisterUnit(AUnit* UnitToDeregister)
{
	if (UnitToDeregister)
	{
		UnitsInPlay.Remove(UnitToDeregister);
	}
}
// DEPLOYMENT QUEUE

bool AGameManager::PurchaseAndQueueUnit(FName UnitRowName, FIntVector SpawnCoords)
{
	if (!UnitDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("PurchaseAndQueueUnit: UnitDataTable is null on GameManager."));
		return false;
	}

	static const FString Context(TEXT("PurchaseAndQueueUnit"));
	const FUnitData* Row = UnitDataTable->FindRow<FUnitData>(UnitRowName, Context);
	if (!Row)
	{
		UE_LOG(LogTemp, Error, TEXT("PurchaseAndQueueUnit: No row named '%s' in UnitDataTable."), *UnitRowName.ToString());
		return false;
	}

	// Spend AP immediately
	if (!TrySpendActionPoints(Row->Action_Cost))
	{
		UE_LOG(LogTemp, Warning, TEXT("PurchaseAndQueueUnit: Not enough AP. Cost=%d AP=%d"), Row->Action_Cost, ActionPoints);
		return false;
	}

	FPendingUnitDeployment Deployment;
	Deployment.UnitRowName = UnitRowName;
	Deployment.SpawnCoords = SpawnCoords;
	Deployment.TurnsRemaining = Row->Turns_To_Deploy;
	Deployment.TurnQueued = CurrentTurn;

	PendingDeployments.Add(Deployment);

	OnDeploymentQueueChanged.Broadcast();
	OnUnitQueued_BP(Deployment);

	UE_LOG(LogTemp, Log, TEXT("Queued unit '%s' at (%d,%d,%d) with TurnsRemaining=%d"),
		*UnitRowName.ToString(), SpawnCoords.X, SpawnCoords.Y, SpawnCoords.Z, Deployment.TurnsRemaining);

	return true;
}

void AGameManager::ClearDeploymentQueue()
{
	PendingDeployments.Empty();
	OnDeploymentQueueChanged.Broadcast();
}

void AGameManager::ProcessDeploymentQueue()
{
	// iterate backwards so RemoveAt is safe
	for (int32 i = PendingDeployments.Num() - 1; i >= 0; --i)
	{
		FPendingUnitDeployment& Entry = PendingDeployments[i];
		Entry.TurnsRemaining -= 1;

		if (Entry.TurnsRemaining <= 0)
		{
			AUnit* NewUnit = DeployUnitNow(Entry);

			// Remove entry regardless
			PendingDeployments.RemoveAt(i);

			OnDeploymentQueueChanged.Broadcast();

			if (NewUnit)
			{
				OnUnitDeployed_BP(NewUnit, Entry.UnitRowName, Entry.SpawnCoords);
			}
		}
	}

	// If nothing deployed, we still changed TurnsRemaining, so UI will want refresh:
	if (PendingDeployments.Num() > 0)
	{
		OnDeploymentQueueChanged.Broadcast();
	}
}

AUnit* AGameManager::DeployUnitNow(const FPendingUnitDeployment& Deployment)
{
	if (!UnitDataTable)
	{
		return nullptr;
	}

	static const FString Context(TEXT("DeployUnitNow"));
	const FUnitData* Row = UnitDataTable->FindRow<FUnitData>(Deployment.UnitRowName, Context);
	if (!Row || !Row->Unit_BP)
	{
		UE_LOG(LogTemp, Error, TEXT("DeployUnitNow: Missing row or Unit_BP for '%s'"), *Deployment.UnitRowName.ToString());
		return nullptr;
	}

	const FVector TempLocation(0.f, 0.f, 200.f);
	const FTransform SpawnTM(FRotator::ZeroRotator, TempLocation);

	AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(Row->Unit_BP, SpawnTM);
	if (!NewUnit)
	{
		return nullptr;
	}

	// Apply the DT row
	NewUnit->ApplyDataFromRowName(Deployment.UnitRowName);

	// Snap to requested hex tile (this also updates GridCoordinates inside SetCurrentTile)
	if (TileManager)
	{
		if (ATile* const* FoundTile = TileManager->TileLookup.Find(Deployment.SpawnCoords))
		{
			NewUnit->SetCurrentTile(*FoundTile);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DeployUnitNow: No tile at (%d,%d,%d). Leaving unit at temp location."),
				Deployment.SpawnCoords.X, Deployment.SpawnCoords.Y, Deployment.SpawnCoords.Z);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Deployed unit '%s' at (%d,%d,%d)"),
		*Deployment.UnitRowName.ToString(),
		Deployment.SpawnCoords.X, Deployment.SpawnCoords.Y, Deployment.SpawnCoords.Z);

	return NewUnit;
}
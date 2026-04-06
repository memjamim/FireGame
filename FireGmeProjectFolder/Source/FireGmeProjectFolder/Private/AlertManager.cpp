#include "AlertManager.h"

#include "GameManager.h"
#include "Tile.h"
#include "TileManager.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	struct FAlertSpawnCandidate
	{
		FName RowName = NAME_None;
		const FAlertData* Data = nullptr;
		TArray<ATile*> ValidTiles;
		int32 Weight = 1;
	};
}

AAlertManager::AAlertManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAlertManager::BeginPlay()
{
	Super::BeginPlay();
	CacheManagerReferences();
}

void AAlertManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAlertManager::CacheManagerReferences()
{
	if (!GameManager)
	{
		GameManager = AGameManager::GetGameManager(this);
	}

	if (!TileManager)
	{
		TArray<AActor*> FoundTileManagers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATileManager::StaticClass(), FoundTileManagers);
		if (FoundTileManagers.Num() > 0)
		{
			TileManager = Cast<ATileManager>(FoundTileManagers[0]);
		}
	}

	return (GameManager != nullptr && TileManager != nullptr);
}

bool AAlertManager::TrySpawnRandomAlert()
{
	if (!CacheManagerReferences())
	{
		UE_LOG(LogTemp, Warning, TEXT("TrySpawnRandomAlert: Missing manager references."));
		return false;
	}

	if (!AlertDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("TrySpawnRandomAlert: AlertDataTable is null."));
		return false;
	}

	if (ActiveAlerts.Num() >= MaxConcurrentAlerts)
	{
		return false;
	}

	TArray<FName> RowNames = AlertDataTable->GetRowNames();
	if (RowNames.Num() == 0)
	{
		return false;
	}

	TArray<FAlertSpawnCandidate> Candidates;
	int32 TotalWeight = 0;

	static const FString Context(TEXT("TrySpawnRandomAlert"));

	for (const FName RowName : RowNames)
	{
		const FAlertData* Data = AlertDataTable->FindRow<FAlertData>(RowName, Context);
		if (!Data)
		{
			continue;
		}

		if (Data->Options.Num() <= 0)
		{
			continue;
		}

		const int32 EffectiveWeight = FMath::Max(Data->Weight, 0);
		if (EffectiveWeight == 0)
		{
			continue;
		}

		TArray<ATile*> ValidTiles;
		for (ATile* Tile : TileManager->RegisteredTiles)
		{
			if (!IsValid(Tile))
			{
				continue;
			}

			if (IsTileAllowedForAlert(Tile, *Data))
			{
				ValidTiles.Add(Tile);
			}
		}

		if (ValidTiles.Num() <= 0)
		{
			continue;
		}

		FAlertSpawnCandidate Candidate;
		Candidate.RowName = RowName;
		Candidate.Data = Data;
		Candidate.ValidTiles = MoveTemp(ValidTiles);
		Candidate.Weight = EffectiveWeight;

		TotalWeight += EffectiveWeight;
		Candidates.Add(MoveTemp(Candidate));
	}

	if (Candidates.Num() == 0 || TotalWeight <= 0)
	{
		return false;
	}

	int32 Roll = FMath::RandRange(1, TotalWeight);
	const FAlertSpawnCandidate* ChosenCandidate = nullptr;

	for (const FAlertSpawnCandidate& Candidate : Candidates)
	{
		Roll -= Candidate.Weight;
		if (Roll <= 0)
		{
			ChosenCandidate = &Candidate;
			break;
		}
	}

	if (!ChosenCandidate || ChosenCandidate->ValidTiles.Num() == 0 || !ChosenCandidate->Data)
	{
		return false;
	}

	ATile* ChosenTile = ChosenCandidate->ValidTiles[FMath::RandRange(0, ChosenCandidate->ValidTiles.Num() - 1)];
	if (!IsValid(ChosenTile))
	{
		return false;
	}

	FActiveAlertInstance NewInstance;
	NewInstance.InstanceId = NextAlertInstanceId++;
	NewInstance.AlertRowName = ChosenCandidate->RowName;
	NewInstance.TileCoords = ChosenTile->GridCoordinates;
	NewInstance.SpawnTurn = GameManager ? GameManager->CurrentTurn : 0;
	NewInstance.TurnsRemaining = FMath::Max(1, ChosenCandidate->Data->TurnsToRespond);
	NewInstance.bResolved = false;

	ActiveAlerts.Add(NewInstance);

	OnAlertSpawned.Broadcast(NewInstance.InstanceId);
	OnAlertSpawned_BP(NewInstance, *ChosenCandidate->Data);

	UE_LOG(LogTemp, Log, TEXT("Alert spawned. InstanceId=%d Row=%s Tile=(%d,%d,%d) TurnsRemaining=%d"),
		NewInstance.InstanceId,
		*NewInstance.AlertRowName.ToString(),
		NewInstance.TileCoords.X, NewInstance.TileCoords.Y, NewInstance.TileCoords.Z,
		NewInstance.TurnsRemaining);

	return true;
}

bool AAlertManager::ResolveAlertOption(int32 AlertInstanceId, int32 OptionIndex)
{
	if (!CacheManagerReferences())
	{
		return false;
	}

	if (!AlertDataTable)
	{
		return false;
	}

	const int32 Index = FindActiveAlertIndexById(AlertInstanceId);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	FActiveAlertInstance& Instance = ActiveAlerts[Index];
	if (Instance.bResolved)
	{
		return false;
	}

	static const FString Context(TEXT("ResolveAlertOption"));
	const FAlertData* Data = AlertDataTable->FindRow<FAlertData>(Instance.AlertRowName, Context);
	if (!Data)
	{
		return false;
	}

	if (!Data->Options.IsValidIndex(OptionIndex))
	{
		return false;
	}

	const FAlertOptionData& OptionData = Data->Options[OptionIndex];

	if (!DoesUnitRequirementPass(Instance.TileCoords, OptionData.UnitRequirement))
	{
		return false;
	}

	if (!TryApplyActionPointCost(OptionData.ActionPointCost))
	{
		return false;
	}

	if (!ApplyOptionEffect(Instance, OptionData))
	{
		return false;
	}

	Instance.bResolved = true;

	OnAlertResolved.Broadcast(Instance.InstanceId);
	OnAlertResolved_BP(Instance, OptionIndex, OptionData);

	UE_LOG(LogTemp, Log, TEXT("Alert resolved. InstanceId=%d Option=%d"), Instance.InstanceId, OptionIndex);

	RemoveAlertAtIndex(Index);
	return true;
}

void AAlertManager::ProcessTurnStart()
{
	if (!AlertDataTable)
	{
		return;
	}

	static const FString Context(TEXT("ProcessTurnStart"));

	for (int32 i = ActiveAlerts.Num() - 1; i >= 0; --i)
	{
		FActiveAlertInstance& Instance = ActiveAlerts[i];
		if (Instance.bResolved)
		{
			continue;
		}

		Instance.TurnsRemaining -= 1;

		if (Instance.TurnsRemaining > 0)
		{
			continue;
		}

		const int32 ExpiredInstanceId = Instance.InstanceId;
		const FAlertData* Data = AlertDataTable->FindRow<FAlertData>(Instance.AlertRowName, Context);
		if (Data)
		{
			OnAlertExpired_BP(Instance, *Data);
		}

		OnAlertExpired.Broadcast(ExpiredInstanceId);

		UE_LOG(LogTemp, Log, TEXT("Alert expired. InstanceId=%d Row=%s"),
			ExpiredInstanceId, *Instance.AlertRowName.ToString());

		RemoveAlertAtIndex(i);
	}
}

bool AAlertManager::GetAlertDisplayData(int32 AlertInstanceId, FActiveAlertInstance& OutInstance, FAlertData& OutData) const
{
	if (!AlertDataTable)
	{
		return false;
	}

	const int32 Index = FindActiveAlertIndexById(AlertInstanceId);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	static const FString Context(TEXT("GetAlertDisplayData"));
	const FAlertData* Data = AlertDataTable->FindRow<FAlertData>(ActiveAlerts[Index].AlertRowName, Context);
	if (!Data)
	{
		return false;
	}

	OutInstance = ActiveAlerts[Index];
	OutData = *Data;
	return true;
}

bool AAlertManager::HasUnresolvedAlerts() const
{
	for (const FActiveAlertInstance& Instance : ActiveAlerts)
	{
		if (!Instance.bResolved)
		{
			return true;
		}
	}

	return false;
}

bool AAlertManager::IsTileAllowedForAlert(const ATile* Tile, const FAlertData& AlertData) const
{
	if (!IsValid(Tile))
	{
		return false;
	}

	if (AlertData.AllowedTileTypes.Num() == 0)
	{
		return true;
	}

	if (AlertData.AllowedTileTypes.Contains(Tile->TileRowName))
	{
		return true;
	}

	const FName TileIdName(*FString::FromInt(Tile->TileID));
	return AlertData.AllowedTileTypes.Contains(TileIdName);
}

bool AAlertManager::DoesUnitRequirementPass(const FIntVector& AlertTileCoords, const FAlertUnitRequirement& Requirement) const
{
	if (Requirement.RequiredUnitID <= 0)
	{
		return true;
	}

	TArray<AActor*> FoundUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), FoundUnits);

	for (AActor* Actor : FoundUnits)
	{
		AUnit* Unit = Cast<AUnit>(Actor);
		if (!Unit)
		{
			continue;
		}

		if (Unit->UnitData.ID != Requirement.RequiredUnitID)
		{
			continue;
		}

		const int32 Distance = AUnit::GetCubeDistance(Unit->GridCoordinates, AlertTileCoords);
		if (Distance <= Requirement.RequiredRadius)
		{
			return true;
		}
	}

	return false;
}

bool AAlertManager::ApplyOptionEffect(const FActiveAlertInstance& Instance, const FAlertOptionData& OptionData)
{
	if (!GameManager)
	{
		return false;
	}

	switch (OptionData.EffectType)
	{
	case EAlertEffectType::None:
		return true;

	case EAlertEffectType::AddActionPoints:
		GameManager->AddActionPoints(FMath::Max(0, OptionData.EffectMagnitude));
		return true;

	case EAlertEffectType::RemoveActionPoints:
		return GameManager->TrySpendActionPoints(FMath::Max(0, OptionData.EffectMagnitude));

	case EAlertEffectType::AddCityHealth:
		GameManager->CityHealth += FMath::Max(0, OptionData.EffectMagnitude);
		return true;

	case EAlertEffectType::RemoveCityHealth:
		GameManager->CityHealth = FMath::Max(0, GameManager->CityHealth - FMath::Max(0, OptionData.EffectMagnitude));
		return true;

	case EAlertEffectType::ChangeWindDirection:
		GameManager->SetWindDirection(GameManager->WindDirection + OptionData.EffectMagnitude);
		return true;

	case EAlertEffectType::SpawnFire:
		if (!TileManager)
		{
			return false;
		}

		if (ATile* const* FoundTile = TileManager->TileLookup.Find(Instance.TileCoords))
		{
			TileManager->IgniteTile(*FoundTile);
			return true;
		}
		return false;

	case EAlertEffectType::SpawnUnit:
		// Use BP hook for project-specific spawn mapping from EffectPayload / EffectMagnitude.
		OnAlertCustomEffect_BP(Instance, OptionData);
		return true;

	case EAlertEffectType::Custom:
		OnAlertCustomEffect_BP(Instance, OptionData);
		return true;

	default:
		return false;
	}
}

bool AAlertManager::TryApplyActionPointCost(int32 ActionPointCost)
{
	if (!GameManager)
	{
		return false;
	}

	const int32 Cost = FMath::Max(0, ActionPointCost);
	if (Cost == 0)
	{
		return true;
	}

	return GameManager->TrySpendActionPoints(Cost);
}

int32 AAlertManager::FindActiveAlertIndexById(int32 AlertInstanceId) const
{
	for (int32 i = 0; i < ActiveAlerts.Num(); ++i)
	{
		if (ActiveAlerts[i].InstanceId == AlertInstanceId)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

void AAlertManager::RemoveAlertAtIndex(int32 Index)
{
	if (ActiveAlerts.IsValidIndex(Index))
	{
		ActiveAlerts.RemoveAt(Index);
	}
}
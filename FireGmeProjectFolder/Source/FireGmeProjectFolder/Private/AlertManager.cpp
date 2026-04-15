#include "AlertManager.h"

#include "GameManager.h"
#include "Tile.h"
#include "TileManager.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"

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
	HandleAlertClickFromCursor();
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

			// Avoid selecting a tile that already has an active alert.
			if (TileCoordsToAlertId.Contains(Tile->GridCoordinates))
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
	RegisterAlertTileLink(NewInstance.InstanceId, NewInstance.TileCoords);

	OnAlertSpawned.Broadcast(NewInstance.InstanceId);
	OnAlertSpawned_BP(NewInstance, *ChosenCandidate->Data);

	UE_LOG(LogTemp, Log, TEXT("Alert spawned. InstanceId=%d Row=%s Tile=(%d,%d,%d) TurnsRemaining=%d"),
		NewInstance.InstanceId,
		*NewInstance.AlertRowName.ToString(),
		NewInstance.TileCoords.X, NewInstance.TileCoords.Y, NewInstance.TileCoords.Z,
		NewInstance.TurnsRemaining);

	return true;
}

bool AAlertManager::EvaluateOptionRequirements(const FActiveAlertInstance& Instance, const FAlertOptionData& OptionData, FText& OutFailureReason) const
{
	if (!GameManager)
	{
		OutFailureReason = FText::FromString(TEXT("Game manager not available."));
		return false;
	}

	if (OptionData.ActionPointCost > GameManager->ActionPoints)
	{
		OutFailureReason = FText::FromString(TEXT("Not enough Action Points."));
		return false;
	}

	if (GameManager->CityHealth < OptionData.MinimumCityHealth || GameManager->CityHealth > OptionData.MaximumCityHealth)
	{
		OutFailureReason = FText::FromString(TEXT("City health requirement not met."));
		return false;
	}

	if (GameManager->CurrentTurn < OptionData.MinimumTurn || GameManager->CurrentTurn > OptionData.MaximumTurn)
	{
		OutFailureReason = FText::FromString(TEXT("Turn requirement not met."));
		return false;
	}

	if (OptionData.RequiredWindDirection >= 0 && GameManager->WindDirection != OptionData.RequiredWindDirection)
	{
		OutFailureReason = FText::FromString(TEXT("Wind direction requirement not met."));
		return false;
	}

	if (!DoesUnitRequirementPass(Instance.TileCoords, OptionData.UnitRequirement))
	{
		OutFailureReason = FText::FromString(TEXT("Required unit is not in range."));
		return false;
	}

	OutFailureReason = FText::GetEmpty();
	return true;
}

bool AAlertManager::ResolveAlertOption(int32 AlertInstanceId, int32 OptionIndex)
{
	if (!CacheManagerReferences() || !AlertDataTable)
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
	if (!Data || !Data->Options.IsValidIndex(OptionIndex))
	{
		return false;
	}

	const FAlertOptionData& OptionData = Data->Options[OptionIndex];

	FText FailureReason;
	if (!EvaluateOptionRequirements(Instance, OptionData, FailureReason))
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

bool AAlertManager::CanResolveAlertOption(int32 AlertInstanceId, int32 OptionIndex, FText& OutFailureReason) const
{
	if (!AlertDataTable)
	{
		OutFailureReason = FText::FromString(TEXT("Alert data table is missing."));
		return false;
	}

	const int32 Index = FindActiveAlertIndexById(AlertInstanceId);
	if (Index == INDEX_NONE)
	{
		OutFailureReason = FText::FromString(TEXT("Alert not found."));
		return false;
	}

	const FActiveAlertInstance& Instance = ActiveAlerts[Index];
	if (Instance.bResolved)
	{
		OutFailureReason = FText::FromString(TEXT("Alert is already resolved."));
		return false;
	}

	static const FString Context(TEXT("CanResolveAlertOption"));
	const FAlertData* Data = AlertDataTable->FindRow<FAlertData>(Instance.AlertRowName, Context);
	if (!Data || !Data->Options.IsValidIndex(OptionIndex))
	{
		OutFailureReason = FText::FromString(TEXT("Invalid option."));
		return false;
	}

	const FAlertOptionData& OptionData = Data->Options[OptionIndex];
	if (!EvaluateOptionRequirements(Instance, OptionData, OutFailureReason))
	{
		if (!OptionData.UnavailableReasonOverride.IsEmpty())
		{
			OutFailureReason = OptionData.UnavailableReasonOverride;
		}
		return false;
	}

	OutFailureReason = FText::GetEmpty();
	return true;
}

void AAlertManager::ProcessTurnStart()
{
	if (!CacheManagerReferences() || !AlertDataTable)
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

		if (const FAlertData* Data = AlertDataTable->FindRow<FAlertData>(Instance.AlertRowName, Context))
		{
			OnAlertExpired_BP(Instance, *Data);
		}

		OnAlertExpired.Broadcast(ExpiredInstanceId);
		RemoveAlertAtIndex(i);
	}

	const int32 CurrentTurn = GameManager ? GameManager->CurrentTurn : 0;
	const bool bForceSpawn = (GuaranteedSpawnEveryNTurns > 0) && ((CurrentTurn - LastSpawnTurn) >= GuaranteedSpawnEveryNTurns);
	const bool bRollSpawn = (FMath::RandRange(1, 100) <= SpawnChancePercent);

	if ((bForceSpawn || bRollSpawn) && TrySpawnRandomAlert())
	{
		LastSpawnTurn = CurrentTurn;
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

bool AAlertManager::GetAlertOptionAvailability(int32 AlertInstanceId, TArray<FAlertOptionAvailability>& OutAvailability) const
{
	OutAvailability.Empty();

	if (!AlertDataTable)
	{
		return false;
	}

	const int32 Index = FindActiveAlertIndexById(AlertInstanceId);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	static const FString Context(TEXT("GetAlertOptionAvailability"));
	const FAlertData* Data = AlertDataTable->FindRow<FAlertData>(ActiveAlerts[Index].AlertRowName, Context);
	if (!Data)
	{
		return false;
	}

	for (const FAlertOptionData& OptionData : Data->Options)
	{
		FAlertOptionAvailability Availability;
		FText FailureReason;
		Availability.bIsAvailable = EvaluateOptionRequirements(ActiveAlerts[Index], OptionData, FailureReason);
		Availability.BlockReason = Availability.bIsAvailable
			? FText::GetEmpty()
			: (!OptionData.UnavailableReasonOverride.IsEmpty() ? OptionData.UnavailableReasonOverride : FailureReason);

		OutAvailability.Add(Availability);
	}

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

	case EAlertEffectType::AddActionPointsPerTurnTemporary:
	case EAlertEffectType::RemoveActionPointsPerTurnTemporary:
		return ApplyTemporaryActionPointIncomeEffect(OptionData);

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
		OnAlertCustomEffect_BP(Instance, OptionData);
		return true;

	case EAlertEffectType::Custom:
		OnAlertCustomEffect_BP(Instance, OptionData);
		return true;

	default:
		return false;
	}
}

bool AAlertManager::ApplyTemporaryActionPointIncomeEffect(const FAlertOptionData& OptionData)
{
	if (!GameManager)
	{
		return false;
	}

	const int32 DurationTurns = FMath::Max(0, OptionData.EffectDurationTurns);
	if (DurationTurns <= 0)
	{
		return false;
	}

	const int32 Magnitude = FMath::Max(0, OptionData.EffectMagnitude);
	if (Magnitude <= 0)
	{
		return false;
	}

	const int32 SignedModifier = (OptionData.EffectType == EAlertEffectType::RemoveActionPointsPerTurnTemporary)
		? -Magnitude
		: Magnitude;

	GameManager->ApplyActionPointsPerTurnModifier(SignedModifier, DurationTurns);
	return true;
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
	if (!ActiveAlerts.IsValidIndex(Index))
	{
		return;
	}

	const int32 InstanceId = ActiveAlerts[Index].InstanceId;
	UnregisterAlertTileLink(InstanceId);

	ActiveAlerts.RemoveAt(Index);
}

void AAlertManager::RegisterAlertTileLink(int32 AlertInstanceId, const FIntVector& TileCoords)
{
	AlertIdToTileCoords.Add(AlertInstanceId, TileCoords);
	TileCoordsToAlertId.Add(TileCoords, AlertInstanceId);
	SetTileAlertIndicator(TileCoords, true);
}

void AAlertManager::UnregisterAlertTileLink(int32 AlertInstanceId)
{
	const FIntVector* FoundCoords = AlertIdToTileCoords.Find(AlertInstanceId);
	if (!FoundCoords)
	{
		return;
	}

	SetTileAlertIndicator(*FoundCoords, false);
	TileCoordsToAlertId.Remove(*FoundCoords);
	AlertIdToTileCoords.Remove(AlertInstanceId);
}

void AAlertManager::SetTileAlertIndicator(const FIntVector& TileCoords, bool bVisible)
{
	if (!TileManager)
	{
		return;
	}

	if (ATile* const* FoundTile = TileManager->TileLookup.Find(TileCoords))
	{
		if (IsValid(*FoundTile))
		{
			(*FoundTile)->SetAlertIndicatorVisible(bVisible);
		}
	}
}

void AAlertManager::HandleAlertClickFromCursor()
{
	if (!CacheManagerReferences())
	{
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
	{
		return;
	}

	const bool bIsLeftMouseDown = PlayerController->IsInputKeyDown(EKeys::LeftMouseButton);
	if (!bIsLeftMouseDown)
	{
		bWasLeftMouseDownLastTick = false;
		return;
	}

	if (bWasLeftMouseDownLastTick)
	{
		return;
	}

	bWasLeftMouseDownLastTick = true;

	FHitResult Hit;
	if (!PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
	{
		return;
	}

	ATile* ClickedTile = Cast<ATile>(Hit.GetActor());
	if (!ClickedTile && Hit.GetComponent())
	{
		ClickedTile = Cast<ATile>(Hit.GetComponent()->GetOwner());
	}

	if (!ClickedTile)
	{
		return;
	}

	const int32* AlertInstanceId = TileCoordsToAlertId.Find(ClickedTile->GridCoordinates);
	if (!AlertInstanceId)
	{
		return;
	}

	OnAlertSelected.Broadcast(*AlertInstanceId);
	OnAlertSelected_BP(*AlertInstanceId);
}

bool AAlertManager::GetActiveAlertById(int32 AlertInstanceId, FActiveAlertInstance& OutAlertInstance) const
{
	const int32 Index = FindActiveAlertIndexById(AlertInstanceId);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	OutAlertInstance = ActiveAlerts[Index];
	return true;
}
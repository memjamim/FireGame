#include "AudioManager.h"
#include "Tile.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"

// Constructor for default values.
AAudioManager::AAudioManager()
{
	PrimaryActorTick.bCanEverTick = false; // We have no need for ticks with music. Well, for now at least.

	// Creation of the audio component that will be used to play sound.
	MusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicComponent"));
	RootComponent = MusicComponent;

	// Don't play any sounds until we desire.
	MusicComponent->bAutoActivate = false;
}

// On the start of the game function.
void AAudioManager::BeginPlay()
{
	Super::BeginPlay();

	if (MusicComponent && BackgroundMusic)
	{
		MusicComponent->SetSound(BackgroundMusic);
		MusicComponent->Play();
	}
}

// Function that plays the End Turn button sound effect.
void AAudioManager::PlayEndTurnButtonSound()
{
	if (EndTurnButtonSound)
	{
		UGameplayStatics::PlaySound2D(this, EndTurnButtonSound); // Non-spatial sounds. Use for buttons and UI clicks.
	}
}

// Function that plays the translating sound for a given Unit.
void AAudioManager::PlayUnitTranslatingSound(AUnit* MovingUnit)
{
	if (MovingUnit->UnitData.ID == static_cast<int32>(UnitTypes::HELICOPTER)) {
		if (EndTurnButtonSound)
		{
			UGameplayStatics::PlaySound2D(this, HelicopterTranslatingSound); // Non-spatial sounds. Use for buttons and UI clicks.
		}
	}
	else if (MovingUnit->UnitData.ID == static_cast<int32>(UnitTypes::RESIDENTIAL_FIREFIGHTER)) {
		if (EndTurnButtonSound)
		{
			UGameplayStatics::PlaySound2D(this, ResidentialFFTranslatingSound); // Non-spatial sounds. Use for buttons and UI clicks.
		}
	}
	else if (MovingUnit->UnitData.ID == static_cast<int32>(UnitTypes::WOODLAND_FIREFIGHTER)) {
		if (EndTurnButtonSound)
		{
			UGameplayStatics::PlaySound2D(this, WoodlandFFTranslatingSound); // Non-spatial sounds. Use for buttons and UI clicks.
		}
	}
}

// Function that plays the settling sound for a given Unit. This is called AFTER setting the currentTile, so currentTile represents the new Tile it just moved to.
void AAudioManager::PlayUnitSettlingSound(AUnit* SettlingUnit)
{
	if (SettlingUnit->UnitData.ID != static_cast<int32>(UnitTypes::HELICOPTER)) {
		if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::FOREST)) {
			if (ForestSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, ForestSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::GRASS)) {
			if (GrassSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, GrassSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::WATER)) {
			if (WoodlandFFTranslatingSound)
			{
				UGameplayStatics::PlaySound2D(this, WoodlandFFTranslatingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::CHARRED)) {
			if (CharredSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, CharredSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::RESIDENTIAL)) {
			if (ResidentialSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, ResidentialSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::MOUNTAIN)) {
			if (MountainSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, MountainSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
	}
}

// Function that plays the fire spreading sound.
void AAudioManager::PlayFireSpreadingSound()
{
	if (FireSpreadingSound)
	{
		if (FireSpreadingSound->CurrentPlayCount.Num() != 1) {
			UGameplayStatics::PlaySound2D(this, FireSpreadingSound); // Non-spatial sounds. Use for buttons and UI clicks.
		}
	}
}
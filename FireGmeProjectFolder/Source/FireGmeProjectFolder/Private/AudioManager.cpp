#include "AudioManager.h"
#include "Tile.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"

// Constructor for default values.
AAudioManager::AAudioManager()
{
	PrimaryActorTick.bCanEverTick = false; // We have no need for ticks with music. Well, for now at least.

	// Creation of the audio components that will be used to play sound. Start with the scene as the root and the rest are the children.
	AudioSceneRootComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioSceneRootComponent"));
	RootComponent = AudioSceneRootComponent;
	AudioSceneRootComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	BackgroundMusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BackgroundMusicComponent"));
	BackgroundMusicComponent->SetupAttachment(AudioSceneRootComponent);
	BackgroundMusicComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	EndTurnButtonSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EndTurnButtonSoundComponent"));
	EndTurnButtonSoundComponent->SetupAttachment(AudioSceneRootComponent);
	EndTurnButtonSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	HelicopterTranslatingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HelicopterTranslatingSoundComponent"));
	HelicopterTranslatingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	HelicopterTranslatingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	ResidentialFFTranslatingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ResidentialFFTranslatingSoundComponent"));
	ResidentialFFTranslatingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	ResidentialFFTranslatingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	WoodlandFFTranslatingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WoodlandFFTranslatingSoundComponent"));
	WoodlandFFTranslatingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	WoodlandFFTranslatingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	GrassSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("GrassSettlingSoundComponent"));
	GrassSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	GrassSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	ResidentialSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ResidentialSettlingSoundComponent"));
	ResidentialSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	ResidentialSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	ForestSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ForestSettlingSoundComponent"));
	ForestSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	ForestSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	CharredSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CharredSettlingSoundComponent"));
	CharredSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	CharredSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	WaterSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WaterSettlingSoundComponent"));
	WaterSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	WaterSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	MountainSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MountainSettlingSoundComponent"));
	MountainSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	MountainSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	FireSpreadingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FireSpreadingSoundComponent"));
	FireSpreadingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	FireSpreadingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	WindDirectionChangeSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WindDirectionChangeSoundComponent"));
	WindDirectionChangeSoundComponent->SetupAttachment(AudioSceneRootComponent);
	WindDirectionChangeSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	AlertNotificationSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AlertNotificationSoundComponent"));
	AlertNotificationSoundComponent->SetupAttachment(AudioSceneRootComponent);
	AlertNotificationSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.
}

// On the start of the game function.
void AAudioManager::BeginPlay()
{
	Super::BeginPlay();

	if (AudioSceneRootComponent && BackgroundMusic)
	{
		BackgroundMusicComponent->SetSound(BackgroundMusic);
		EndTurnButtonSoundComponent->SetSound(EndTurnButtonSound);
		HelicopterTranslatingSoundComponent->SetSound(HelicopterTranslatingSound);
		ResidentialFFTranslatingSoundComponent->SetSound(ResidentialFFTranslatingSound);
		WoodlandFFTranslatingSoundComponent->SetSound(WoodlandFFTranslatingSound);
		GrassSettlingSoundComponent->SetSound(GrassSettlingSound);
		ResidentialSettlingSoundComponent->SetSound(ResidentialSettlingSound);
		ForestSettlingSoundComponent->SetSound(ForestSettlingSound);
		CharredSettlingSoundComponent->SetSound(CharredSettlingSound);
		WaterSettlingSoundComponent->SetSound(WaterSettlingSound);
		MountainSettlingSoundComponent->SetSound(MountainSettlingSound);
		FireSpreadingSoundComponent->SetSound(FireSpreadingSound);
		WindDirectionChangeSoundComponent->SetSound(WindDirectionChangeSound);
		AlertNotificationSoundComponent->SetSound(AlertNotificationSound);
		BackgroundMusicComponent->SetVolumeMultiplier(0.5f);
		BackgroundMusicComponent->Play();
	}
}

// Function that plays the End Turn button sound effect.
void AAudioManager::PlayEndTurnButtonSound()
{
	if (EndTurnButtonSound)
	{
		EndTurnButtonSoundComponent->SetVolumeMultiplier(0.75f);
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
		if (!(FireSpreadingSoundComponent->IsPlaying())) {
			FireSpreadingSoundComponent->SetVolumeMultiplier(0.5f);
			FireSpreadingSoundComponent->Play();
		}
	}
}

// Function that plays the wind direction changing sound.
void AAudioManager::PlayWindDirectionChangeSound()
{
	if (WindDirectionChangeSound)
	{
		if (!(WindDirectionChangeSoundComponent->IsPlaying())) {
			WindDirectionChangeSoundComponent->SetVolumeMultiplier(3.0f);
			WindDirectionChangeSoundComponent->Play();
		}
	}
}

// Function that plays the Alert notification sound.
void AAudioManager::PlayAlertNotificationSound()
{
	if (AlertNotificationSound)
	{
		if (!(AlertNotificationSoundComponent->IsPlaying())) {
			AlertNotificationSoundComponent->Play();
		}
	}
}
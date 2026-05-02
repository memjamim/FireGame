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

	PlaneTranslatingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PlaneTranslatingSoundComponent"));
	PlaneTranslatingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	PlaneTranslatingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	FiretruckTranslatingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FiretruckTranslatingSoundComponent"));
	FiretruckTranslatingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	FiretruckTranslatingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

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

	RockyMountainSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("RockyMountainSettlingSoundComponent"));
	RockyMountainSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	RockyMountainSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	GrassyMountainSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("GrassyMountainSettlingSoundComponent"));
	GrassyMountainSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	GrassyMountainSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	CommunicationsTowerSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CommunicationsTowerSettlingSoundComponent"));
	CommunicationsTowerSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	CommunicationsTowerSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	WaterTowerSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WaterTowerSettlingSoundComponent"));
	WaterTowerSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	WaterTowerSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	FireStationSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FireStationSettlingSoundComponent"));
	FireStationSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	FireStationSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	CharredMountainSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CharredMountainSettlingSoundComponent"));
	CharredMountainSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	CharredMountainSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

	SchoolSettlingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SchoolSettlingSoundComponent"));
	SchoolSettlingSoundComponent->SetupAttachment(AudioSceneRootComponent);
	SchoolSettlingSoundComponent->bAutoActivate = false; // Don't play any sounds until we desire.

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

		// Unit translation sounds.
		HelicopterTranslatingSoundComponent->SetSound(HelicopterTranslatingSound);
		ResidentialFFTranslatingSoundComponent->SetSound(ResidentialFFTranslatingSound);
		WoodlandFFTranslatingSoundComponent->SetSound(WoodlandFFTranslatingSound);
		PlaneTranslatingSoundComponent->SetSound(PlaneTranslatingSound);
		FiretruckTranslatingSoundComponent->SetSound(FiretruckTranslatingSound);

		// Unit settling on a Tile sounds.
		GrassSettlingSoundComponent->SetSound(GrassSettlingSound);
		ResidentialSettlingSoundComponent->SetSound(ResidentialSettlingSound);
		ForestSettlingSoundComponent->SetSound(ForestSettlingSound);
		CharredSettlingSoundComponent->SetSound(CharredSettlingSound);
		WaterSettlingSoundComponent->SetSound(WaterSettlingSound);
		RockyMountainSettlingSoundComponent->SetSound(RockyMountainSettlingSound);
		GrassyMountainSettlingSoundComponent->SetSound(GrassyMountainSettlingSound);
		CommunicationsTowerSettlingSoundComponent->SetSound(CommunicationsTowerSettlingSound);
		WaterTowerSettlingSoundComponent->SetSound(WaterTowerSettlingSound);
		FireStationSettlingSoundComponent->SetSound(FireStationSettlingSound);
		CharredMountainSettlingSoundComponent->SetSound(CharredMountainSettlingSound);
		SchoolSettlingSoundComponent->SetSound(SchoolSettlingSound);

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
		if (HelicopterTranslatingSound)
		{
			UGameplayStatics::PlaySound2D(this, HelicopterTranslatingSound); // Non-spatial sounds. Use for buttons and UI clicks.
		}
	}
	else if (MovingUnit->UnitData.ID == static_cast<int32>(UnitTypes::RESIDENTIAL_FIREFIGHTER)) {
		if (ResidentialFFTranslatingSound)
		{
			UGameplayStatics::PlaySound2D(this, ResidentialFFTranslatingSound); // Non-spatial sounds. Use for buttons and UI clicks.
		}
	}
	else if (MovingUnit->UnitData.ID == static_cast<int32>(UnitTypes::WOODLAND_FIREFIGHTER)) {
		if (WoodlandFFTranslatingSound)
		{
			UGameplayStatics::PlaySound2D(this, WoodlandFFTranslatingSound); // Non-spatial sounds. Use for buttons and UI clicks.
		}
	}
	else if (MovingUnit->UnitData.ID == static_cast<int32>(UnitTypes::FIRE_PLANE)) {
		if (PlaneTranslatingSound)
		{
			UGameplayStatics::PlaySound2D(this, PlaneTranslatingSound); // Non-spatial sounds. Use for buttons and UI clicks.
		}
	}
	else if (MovingUnit->UnitData.ID == static_cast<int32>(UnitTypes::FIRE_TRUCK)) {
		if (FiretruckTranslatingSound)
		{
			UGameplayStatics::PlaySound2D(this, FiretruckTranslatingSound); // Non-spatial sounds. Use for buttons and UI clicks.
		}
	}
}

// Function that plays the settling sound for a given Unit. This is called AFTER setting the currentTile, so currentTile represents the new Tile it just moved to.
void AAudioManager::PlayUnitSettlingSound(AUnit* SettlingUnit)
{
	if (SettlingUnit->UnitData.ID != static_cast<int32>(UnitTypes::HELICOPTER) &&
		SettlingUnit->UnitData.ID != static_cast<int32>(UnitTypes::FIRE_PLANE)) {
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
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::ROCKY_MOUNTAIN)) {
			if (RockyMountainSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, RockyMountainSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::GRASSY_MOUNTAIN)) {
			if (GrassyMountainSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, GrassyMountainSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::COMMUNICATIONS_TOWER)) {
			if (CommunicationsTowerSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, CommunicationsTowerSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::WATER_TOWER)) {
			if (WaterTowerSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, WaterTowerSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::FIRE_STATION)) {
			if (FireStationSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, FireStationSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::CHARRED_MOUNTAIN)) {
			if (CharredMountainSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, CharredMountainSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
			}
		}
		else if (SettlingUnit->CurrentTile->TileID == static_cast<int32>(TileTypes::SCHOOL)) {
			if (SchoolSettlingSound)
			{
				UGameplayStatics::PlaySound2D(this, SchoolSettlingSound); // Non-spatial sounds. Use for buttons and UI clicks.
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
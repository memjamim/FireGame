#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "AudioManager.generated.h"

// Types of Units
UENUM(BlueprintType)
enum class UnitTypes : uint8
{
	FIRST_ENTRY = 0,
	HELICOPTER = 1,
	RESIDENTIAL_FIREFIGHTER = 2,
	WOODLAND_FIREFIGHTER = 3
};

// Types of Tiles
UENUM(BlueprintType)
enum class TileTypes : uint8
{
	FOREST = 0,
	GRASS = 1,
	WATER = 2,
	CHARRED = 3,
	RESIDENTIAL = 4,
	MOUNTAIN = 5,
	COMMUNICATIONSTOWER = 6
};

UCLASS()
class FIREGMEPROJECTFOLDER_API AAudioManager : public AActor
{
	GENERATED_BODY()

public:
	AAudioManager();

protected:
	virtual void BeginPlay() override;

public:

	// Component for audio that will play the music chosen (for the root component).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* AudioSceneRootComponent;

	// Component for audio that will play the music chosen (for the score/backing track).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* BackgroundMusicComponent;

	// The .wav file for the score/backing track assigned in the Blueprint.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* BackgroundMusic;

	// Component for audio that will play the music chosen (for the End Turn button sound).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* EndTurnButtonSoundComponent;

	// The .wav file for the End Turn button sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* EndTurnButtonSound;

	// Component for audio that will play the music chosen (for the Helicopter movement sounds).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* HelicopterTranslatingSoundComponent;

	// The .wav file for the Helicopter translating sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* HelicopterTranslatingSound;

	// Component for audio that will play the music chosen (for the Residential Firefighter movement sounds).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* ResidentialFFTranslatingSoundComponent;

	// The .wav file for the Residential Firefighter translating sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* ResidentialFFTranslatingSound;

	// Component for audio that will play the music chosen (for the Woodland Firefighter movement sounds).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* WoodlandFFTranslatingSoundComponent;

	// The .wav file for the Woodland Firefighter translating sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* WoodlandFFTranslatingSound;

	// Component for audio that will play the music chosen (for the Unit settling on Grass Tile sounds).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* GrassSettlingSoundComponent;

	// The .wav file for a Unit moving onto a Grass Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* GrassSettlingSound;

	// Component for audio that will play the music chosen (for the Unit settling on Residential Tile sounds).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* ResidentialSettlingSoundComponent;

	// The .wav file for a Unit moving onto a Residential Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* ResidentialSettlingSound;

	// Component for audio that will play the music chosen (for the Unit settling on Forest Tile sounds).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* ForestSettlingSoundComponent;

	// The .wav file for a Unit moving onto a Forest Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* ForestSettlingSound;

	// Component for audio that will play the music chosen (for the Unit settling on Charred Tile sounds).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* CharredSettlingSoundComponent;

	// The .wav file for a Unit moving onto a Charred Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* CharredSettlingSound;

	// Component for audio that will play the music chosen (for the Unit settling on Water Tile sounds).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* WaterSettlingSoundComponent;

	// The .wav file for a Unit moving onto a Water Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* WaterSettlingSound;

	// Component for audio that will play the music chosen (for the Unit settling on Mountain Tile sounds).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* MountainSettlingSoundComponent;

	// The .wav file for a Unit moving onto a Mountain Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* MountainSettlingSound;

	// Component for audio that will play the music chosen (for the Communications Tower Tile).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* CommunicationsTowerSettlingSoundComponent;

	// The .wav file for the Communications Tower Tile.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* CommunicationsTowerSettlingSound;

	// Component for audio that will play the music chosen (for the fire spreading sounds).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* FireSpreadingSoundComponent;

	// The .wav file for the fire spreading sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* FireSpreadingSound;

	// Component for audio that will play the music chosen (for the wind direction changing).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* WindDirectionChangeSoundComponent;

	// The .wav file for the wind direction change sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* WindDirectionChangeSound;

	// Component for audio that will play the music chosen (for the Alert notification system).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* AlertNotificationSoundComponent;

	// The .wav file for the Alert notification system.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* AlertNotificationSound;

	// A function used to play the End Turn button sound effect.
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayEndTurnButtonSound();

	// A function used to play the Translating Sounds for any given Unit.
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayUnitTranslatingSound(AUnit* MovingUnit);

	// A function used to play the sounds for a Unit settling onto a given Tile.
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayUnitSettlingSound(AUnit* SettlingUnit);

	// A function used to play the sounds for when fire spreads to a new Tile.
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayFireSpreadingSound();

	// A function used to play the sounds for when the wind direction changes.
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayWindDirectionChangeSound();

	// A function used to play the sounds for when the Alert notification system goes off.
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayAlertNotificationSound();
};
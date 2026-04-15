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
	MOUNTAIN = 5
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
	// Component for audio that will play the music chosen (for the score/backing track).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* MusicComponent;

	// The .wav file for the score/backing track assigned in the Blueprint.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* BackgroundMusic;

	// The .wav file for the End Turn button sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* EndTurnButtonSound;

	// The .wav file for the Helicopter translating sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* HelicopterTranslatingSound;

	// The .wav file for the Residential Firefighter translating sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* ResidentialFFTranslatingSound;

	// The .wav file for the Woodland Firefighter translating sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* WoodlandFFTranslatingSound;

	// The .wav file for a Unit moving onto a Grass Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* GrassSettlingSound;

	// The .wav file for a Unit moving onto a Residential Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* ResidentialSettlingSound;

	// The .wav file for a Unit moving onto a Forest Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* ForestSettlingSound;

	// The .wav file for a Unit moving onto a Charred Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* CharredSettlingSound;

	// The .wav file for a Unit moving onto a Water Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* WaterSettlingSound;

	// The .wav file for a Unit moving onto a Mountain Tile sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* MountainSettlingSound;

	// The .wav file for the fire spreading sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* FireSpreadingSound;

	// The .wav file for the wind direction change sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* WindDirectionChangeSound;

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
};
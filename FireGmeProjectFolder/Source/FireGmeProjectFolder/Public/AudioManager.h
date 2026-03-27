#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "AudioManager.generated.h"

UCLASS()
class FIREGMEPROJECTFOLDER_API AAudioManager : public AActor
{
	GENERATED_BODY()

public:
	AAudioManager();

protected:
	virtual void BeginPlay() override;

public:
	// The singleton used to play button sounds during play. Can be accessed by other .cpp files.
	UFUNCTION(BlueprintCallable, Category = "Audio")
	static AAudioManager* Get(UWorld* World);

	// Component for audio that will play the music chosen (for the score/backing track).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* MusicComponent;

	// The .wav file for the score/backing track assigned in the Blueprint.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* BackgroundMusic;

	// The .wav file for the End Turn button sound effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* EndTurnButtonSound;

	// A function used to play the End Turn button sound effect.
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayEndTurnButtonSound();

private:
	// A reference to the singleton.
	static AAudioManager* SingletonInstance;
};
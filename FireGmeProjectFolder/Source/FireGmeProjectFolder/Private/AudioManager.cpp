#include "AudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"

// Go ahead and initialize the singleton as a null pointer.
AAudioManager* AAudioManager::SingletonInstance = nullptr;

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

// Function that allows you to access the AudioManager singleton. Allows you to use the AudioManager in other .cpp classes.
AAudioManager* AAudioManager::Get(UWorld* World)
{
	if (SingletonInstance)
	{
		return SingletonInstance;
	}

	// Try to find an instance in the world if it is not already existing.
	for (TActorIterator<AAudioManager> It(World); It; ++It)
	{
		SingletonInstance = *It;
		return SingletonInstance;
	}

	// Only print this if there exists no AudioManager.
	UE_LOG(LogTemp, Warning, TEXT("No AudioManagar could be found."));
	return nullptr;
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
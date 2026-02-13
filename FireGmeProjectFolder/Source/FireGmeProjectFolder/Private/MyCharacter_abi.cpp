// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter_abi.h"

// Sets default values
AMyCharacter_abi::AMyCharacter_abi()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyCharacter_abi::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyCharacter_abi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter_abi::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


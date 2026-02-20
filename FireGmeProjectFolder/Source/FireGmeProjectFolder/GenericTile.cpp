// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericTile.h"

// Sets default values
AGenericTile::AGenericTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGenericTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGenericTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGenericTile::SpawnActor()
{
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(actorBPToSpawn, GetActorTransform(), spawnParams);
}
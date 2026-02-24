// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTile.generated.h"

UCLASS()
class FIREGMEPROJECTFOLDER_API AGenericTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGenericTile();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SpawnActor();

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> actorBPToSpawn;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

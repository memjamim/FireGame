// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestConeActor.generated.h"

UCLASS()
class FIREGMEPROJECTFOLDER_API ATestConeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestConeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Cone mesh
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ConeMesh;

	// Function to modify cone
	UFUNCTION(BlueprintCallable, Category="Cone")
	void ChangeCone();

};

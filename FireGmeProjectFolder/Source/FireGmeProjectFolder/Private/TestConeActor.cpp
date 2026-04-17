// Fill out your copyright notice in the Description page of Project Settings.


#include "TestConeActor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATestConeActor::ATestConeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create cone mesh
	ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeMesh"));
	RootComponent = ConeMesh;

}

// Called when the game starts or when spawned
void ATestConeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestConeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestConeActor::ChangeCone()
{

    FVector Current = GetActorScale3D();

    if (Current.X < 2.5f)
    {
        SetActorScale3D(FVector(3, 3, 3));
    }
    else
    {
        SetActorScale3D(FVector(1, 1, 1));
    }
}


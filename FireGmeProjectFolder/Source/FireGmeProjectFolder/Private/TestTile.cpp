#include "TestTile.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ATestTile::ATestTile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create a collision box as root
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	// Create a visible mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Give it an obvious default mesh (Engine cube)
	// This path is the common Engine cube used in many UE examples. :contentReference[oaicite:3]{index=3}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")
	);
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
		Mesh->SetRelativeScale3D(FVector(1.f, 1.f, 0.25f)); // flatter "tile"
	}

	// Let this actor receive input from Player 0 automatically
	AutoReceiveInput = EAutoReceiveInput::Player0;
}

void ATestTile::BeginPlay()
{
	Super::BeginPlay();

	// Create a dynamic material instance so we can change a parameter at runtime
	if (BaseMaterial)
	{
		MID = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		Mesh->SetMaterial(0, MID);
		ApplyColor(ColorA);
	}

	// Bind a key to prove we can do something in-game.
	// Note: enabling input on actors is an official workflow (Blueprint + C++ both). :contentReference[oaicite:4]{index=4}
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		EnableInput(PC);

		if (InputComponent)
		{
			InputComponent->BindKey(EKeys::T, IE_Pressed, this, &ATestTile::ToggleColor);
		}
	}
}

void ATestTile::ToggleColor()
{
	bIsColorB = !bIsColorB;
	ApplyColor(bIsColorB ? ColorB : ColorA);
}

void ATestTile::ApplyColor(const FLinearColor& C)
{
	if (MID)
	{
		MID->SetVectorParameterValue(ColorParamName, C);
	}
}

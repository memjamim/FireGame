#include "Tile.h"

// Sets default values
ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATile::Ignite()
{
	if (bIsBurnable && !bIsBurning)
	{
		bIsBurning = true;
	}
}

void ATile::Extinguish()
{
	bIsBurning = false;
}

bool ATile::ApplyDataFromRowName(FName RowName)
{
	if (!TileDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("TileDataTable is null on tile at (%d,%d,%d)"),
			GridCoordinates.X, GridCoordinates.Y, GridCoordinates.Z);
		return false;
	}

	static const FString Context(TEXT("TileDataLookup"));
	const FTileDataRow* Row = TileDataTable->FindRow<FTileDataRow>(RowName, Context);
	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("No row named %s found in DT_Tile"), *RowName.ToString());
		return false;
	}

	TileID = Row->ID;
	bIsBurnable = Row->Is_Burnable;
	CurrentFireHealth = Row->Tile_Health;
	CommunityHealthCost = Row->Community_Health_Cost;

	UpdateTileVisuals();
	return true;
}

bool ATile::ApplyDataFromID(int32 InTileID)
{
	if (!TileDataTable)
	{
		return false;
	}

	TArray<FTileDataRow*> AllRows;
	static const FString Context(TEXT("TileDataLookup"));
	TileDataTable->GetAllRows(Context, AllRows);

	for (const FTileDataRow* Row : AllRows)
	{
		if (Row && Row->ID == InTileID)
		{
			TileID = Row->ID;
			bIsBurnable = Row->Is_Burnable;
			CurrentFireHealth = Row->Tile_Health;
			CommunityHealthCost = Row->Community_Health_Cost;

			UpdateTileVisuals();
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No row with ID %d found in DT_Tile"), InTileID);
	return false;
}
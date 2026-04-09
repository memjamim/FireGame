#include "ProceduralGeneration.h"
#include "Tile.h"
#include "TileManager.h"
#include "Engine/World.h"

// Helper function that creates a cluster for Residential Tiles.
static void GrowCluster(const TArray<FIntVector>& Directions, TSet<FIntVector>& AvailableTiles, TSet<FIntVector>& OutCluster,
						const FIntVector& Start, int32 TargetSize)
{
	TArray<FIntVector> Cluster;
	Cluster.Add(Start);
	OutCluster.Add(Start);

	// Continue to add Residential Tiles to the cluster until we reach the desired cluster size.
	while (OutCluster.Num() < TargetSize && Cluster.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, Cluster.Num() - 1);
		FIntVector Current = Cluster[Index];

		FIntVector Dir = Directions[FMath::RandRange(0, Directions.Num() - 1)];
		FIntVector Next = Current + Dir;

		if (AvailableTiles.Contains(Next) && !OutCluster.Contains(Next))
		{
			OutCluster.Add(Next);
			Cluster.Add(Next);
		}
	}
}

// Procedurally generates a map for the game.
void ProceduralGeneration::GenerateMap(UWorld* World, TSubclassOf<ATile> TileClass, int32 NumberOfTiles, int32 GrassTileID,
										int32 ResidentialTileID, int32 ForestTileID, ATileManager* TileManager)
{
	if (!World || !TileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sanity Check: Invalid World or TileClass variable."));
		return;
	}

	// The Cube Coordinate directions for Tiles.
	const TArray<FIntVector> Directions =
	{
		FIntVector(1, -1,  0),
		FIntVector(1,  0, -1),
		FIntVector(0,  1, -1),
		FIntVector(-1,  1,  0),
		FIntVector(-1,  0,  1),
		FIntVector(0, -1,  1)
	};

	TSet<FIntVector> ChosenTiles; // All of the Tiles that exist in the map.
	TArray<FIntVector> MapEdgeTiles; // The Tiles that exist along the edge that allow for offshoots/outwards growth into "coastlines".

	// Setup the array/set with the starting Tile at the origin.
	FIntVector Start(0, 0, 0);
	ChosenTiles.Add(Start);
	MapEdgeTiles.Add(Start);

	// The chance for a tile to grow outwards from a given tile. Helps create something that is more random and less fixed.
	const float GrowthChance = 0.7f;

	// Loop that builds the map outwards. Will continue until we reach the Tile limit set in TileManager.cpp's "Begin" function.
	while (ChosenTiles.Num() < NumberOfTiles && MapEdgeTiles.Num() > 0)
	{
		// Choose a random Tile that may exist.
		int32 Random = FMath::RandRange(0, MapEdgeTiles.Num() - 1);
		FIntVector Current = MapEdgeTiles[Random];

		for (const FIntVector& Dir : Directions)
		{
			if (FMath::FRand() > GrowthChance) // Decides if the Tile in a given direction has a chance to exist.
			{
				continue;
			}

			FIntVector NewCoord = Current + Dir; // If this Tile can exist...

			if (!ChosenTiles.Contains(NewCoord)) // ...add the Tile's coordinates to the arrays.
			{
				ChosenTiles.Add(NewCoord);
				MapEdgeTiles.Add(NewCoord);
			}
		}

		// Gurantees that at least one direction will receive a new Tile's coordiantes. 
		// This prevents a map that potentially never grows or grows too slowly.
		FIntVector Dir = Directions[FMath::RandRange(0, Directions.Num() - 1)];
		FIntVector NewCoord = Current + Dir;

		if (!ChosenTiles.Contains(NewCoord))
		{
			ChosenTiles.Add(NewCoord);
			MapEdgeTiles.Add(NewCoord);
		}
	}

	// Creates the Tiles in the game world based on the coordinates we chose earlier.
	for (const FIntVector& GridCoord : ChosenTiles)
	{
		FActorSpawnParameters Params;

		ATile* NewTile = World->SpawnActor<ATile>(TileClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

		if (!NewTile)
		{
			continue;
		}

		NewTile->GridCoordinates = GridCoord;

		NewTile->TileManager = TileManager;

		FVector WorldLocation = NewTile->ConvertGridToWorldCoordinates(GridCoord);
		NewTile->SetActorLocation(WorldLocation);

		NewTile->ApplyDataFromID(GrassTileID); // Ensures that all Tiles created are initially Grass tiles.

		if (TileManager)
		{
			TileManager->RegisterTile(NewTile); // Register this tile with the Manager.
		}
	}

	if (!TileManager)
	{
		return;
	}

	// Converting to an Array allows for us to randomly select Tiles for creating clusters.
	TArray<FIntVector> TileArray = ChosenTiles.Array();

	// Number of Residential Tile clusters we allow the map to have.
	int32 ResidentialClusters = 1;

	for (int i = 0; i < ResidentialClusters; i++)
	{
		FIntVector Seed = TileArray[FMath::RandRange(0, TileArray.Num() - 1)]; // Choose a random Tile...
		int32 ClusterSize = 2; //FMath::RandRange(2, 7); // ...choose a random size for the Residential cluster...

		TSet<FIntVector> Cluster; // ...create a set for the Residential Tile coordinates...

		GrowCluster(Directions, ChosenTiles, Cluster, Seed, ClusterSize); // ...and grow the Residential cluster using the helper function.

		// Fully applies the Residential Tiles ontop of the Grass Tiles, overwriting them entirely.
		for (const FIntVector& Coord : Cluster)
		{
			if (ATile* Tile = TileManager->TileLookup.FindRef(Coord))
			{

				Tile->ApplyDataFromID(ResidentialTileID);
			}
		}
	}

	// Number of Forest Tile clusters we allow the map to have.
	int32 ForestClusters = 0;

	for (int i = 0; i < ForestClusters; i++)
	{
		FIntVector Seed = TileArray[FMath::RandRange(0, TileArray.Num() - 1)]; // Choose a random Tile...
		int32 ClusterSize = FMath::RandRange(5, 15); // ...choose a random size for the Forest cluster...

		TSet<FIntVector> Cluster; // ...create a set for the Forest Tile coordinates...
		TArray<FIntVector> ForestEdge; // ...create an array of the edge Tile coordinates of the cluster...

		Cluster.Add(Seed); // ...add in the starting Forest Tile...
		ForestEdge.Add(Seed);

		// ...and grow the Forest Tiles. The logic for growing Forests is different than growing Residentials.
		while (Cluster.Num() < ClusterSize && ForestEdge.Num() > 0)
		{
			int32 Index = FMath::RandRange(0, ForestEdge.Num() - 1);
			FIntVector Current = ForestEdge[Index];

			for (const FIntVector& Dir : Directions) // In every direction...
			{
				float SpreadChance = (ClusterSize <= 8) ? 0.9f : 0.6f; // ...grow Forests depending on the number desired.
				// Small Forest clusters leads to a higher chance that Grass turns into Forest close by the start (0.9f).
				// Large Forest clusters leads to a smaller chance that Grass turns into Forest close by the start (0.6f).

				if (FMath::FRand() > SpreadChance)
				{
					continue;
				}

				// Get the next direction to build a Forest in.
				FIntVector Next = Current + Dir;

				// Add this new set of coordinates to the array/set.
				if (ChosenTiles.Contains(Next) && !Cluster.Contains(Next))
				{
					Cluster.Add(Next);
					ForestEdge.Add(Next);
				}
			}
		}

		// Apply the data for the new Forests.
		for (const FIntVector& Coord : Cluster)
		{
			if (ATile* Tile = TileManager->TileLookup.FindRef(Coord))
			{

				Tile->ApplyDataFromID(ForestTileID);
			}
		}
	}

	// Show how many generated Tiles there are.
	UE_LOG(LogTemp, Log, TEXT("Generated %d tiles."), ChosenTiles.Num());
}
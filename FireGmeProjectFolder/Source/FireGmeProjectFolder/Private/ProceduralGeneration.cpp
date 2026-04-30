#include "ProceduralGeneration.h"
#include "Tile.h"
#include "TileManager.h"
#include "Engine/World.h"

// Helper function that creates a cluster for Mountain Tiles.
static void GrowStrip(const TArray<FIntVector>& Directions, const TSet<FIntVector>& AvailableTiles, TSet<FIntVector>& OutStrip,
					const FIntVector& Start, int32 TargetLength)
{
	FIntVector CurrentLocation = Start;

	// Choose a random initial direction between the six we are provided.
	FIntVector Direction = Directions[FMath::RandRange(0, Directions.Num() - 1)];

	OutStrip.Add(CurrentLocation);

	// Continue to add Mountain Tiles to the desired path until we reach the desired length of this strip.
	for (int32 i = 1; i < TargetLength; i++)
	{
		// Apply a small chance to move the strip in a new direction. Lower means straighter strips, higher results in more random directions.
		if (FMath::FRand() < 0.25f)
		{
			Direction = Directions[FMath::RandRange(0, Directions.Num() - 1)];
		}

		FIntVector NextTile = CurrentLocation + Direction;

		// If we find an invalid location (edge of the map or so on), we will stop growing.
		if (!AvailableTiles.Contains(NextTile))
		{
			break;
		}

		OutStrip.Add(NextTile);
		CurrentLocation = NextTile;
	}
}

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
										int32 ResidentialTileID, int32 ForestTileID, int32 NonBurnableMountainTileID,
										int32 BurnableMountainTileID, int32 CommunicationsTowerID, int32 WaterTowerID,
										int32 FireStationID, ATileManager* TileManager)
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
			TileManager->GrassTiles.Add(NewTile); // Also, add this new Grass Tile to the entire list of Grass Tiles (150 to start).
		}
	}

	if (!TileManager)
	{
		return;
	}

	// Converting to an Array allows for us to randomly select Tiles for creating clusters.
	TArray<FIntVector> TileArray = ChosenTiles.Array();

	// Number of Forest Tile clusters we allow the map to have.
	int32 ForestClusters = 4;

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
				if (Tile->TileID == GrassTileID) {
					TileManager->GrassTiles.Remove(Tile);
				}
				Tile->ApplyDataFromID(ForestTileID);
				TileManager->ForestTiles.Add(Tile);
			}
		}
	}

	// Number of Mountain Tile strips we allow the map to have.
	int32 MountainStrips = 4;

	// Check to see if the Communications Tower has been set into the map or not.
	bool CommunicationsTowerPlaced = false;

	for (int i = 0; i < MountainStrips; i++)
	{
		FIntVector Seed = TileArray[FMath::RandRange(0, TileArray.Num() - 1)]; // Choose a random Tile...

		int32 MountainStripLength = FMath::RandRange(3, 6); // ...choose a random size for the Mountain strip...

		TSet<FIntVector> MountainStrip;

		GrowStrip(Directions, ChosenTiles, MountainStrip, Seed, MountainStripLength); // ...and grow the Mountain strip using the helper function.

		int32 stripSize = MountainStrip.Num(); // The number of element in this given Mountain Strip.

		// Fully applies the Mountain Tiles ontop of the Grass or Forest Tiles, overwriting them entirely.
		for (const FIntVector& Coordinate : MountainStrip)
		{
			if (ATile* Tile = TileManager->TileLookup.FindRef(Coordinate))
			{
				if (Tile->TileID == ForestTileID) {
					TileManager->ForestTiles.Remove(Tile);
				}
				if (Tile->TileID == GrassTileID) {
					TileManager->GrassTiles.Remove(Tile);
				}

				// Create a random chance for any given Non-Burnable Mountain to turn into a Burnable Mountain.
				float burnableMountainChance = FMath::FRand();

				if (burnableMountainChance < 0.15f) { // If the chance is too low...
					burnableMountainChance = burnableMountainChance + 0.15f; // ...raise it just a bit.
				}

				if (burnableMountainChance >= 0.50f && Tile->TileID != CommunicationsTowerID) { // We have a 65% chance for a given Non-Burnable Mountain Tile...
					Tile->ApplyDataFromID(BurnableMountainTileID); // ...to turn into a Burnable Mountain Tile.
				}
				else if (Tile->TileID != CommunicationsTowerID){ // But if it falls in the 35% chance...
					Tile->ApplyDataFromID(NonBurnableMountainTileID); // ...we will instead see a Non-Burnable Mountain Tile.
				}

				if (!CommunicationsTowerPlaced) { // Now handle the Communications Tower Tile. If we haven't placed it...
					Tile->ApplyDataFromID(CommunicationsTowerID); // ...turn the given Tile into the Communications Tower Tile.
					CommunicationsTowerPlaced = true;
				}
			}
		}
	}

	// Number of Residential Tile clusters we allow the map to have.
	int32 ResidentialClusters = 5;

	for (int i = 0; i < ResidentialClusters; i++)
	{
		FIntVector Seed = TileArray[FMath::RandRange(0, TileArray.Num() - 1)]; // Choose a random Tile...
		int32 ClusterSize = FMath::RandRange(2, 7); // ...choose a random size for the Residential cluster...

		TSet<FIntVector> Cluster; // ...create a set for the Residential Tile coordinates...

		GrowCluster(Directions, ChosenTiles, Cluster, Seed, ClusterSize); // ...and grow the Residential cluster using the helper function.

		bool clusterFireStationPlaced = false;

		int fireStationTalley = 0;

		// Fully applies the Residential Tiles ontop of the Grass, Forest, or Mountain Tiles, overwriting them entirely.
		for (const FIntVector& Coordinate : Cluster)
		{
			if (ATile* Tile = TileManager->TileLookup.FindRef(Coordinate)) {
				// Create a random chance for any given Residential Tile to turn into a Fire Station.
				float fireStationChance = FMath::FRand();

				if (Tile->TileID != FireStationID && Tile->TileID != CommunicationsTowerID) { // Make sure we are not replacing a Fire Station.
					if (fireStationChance >= 0.25f) { // If we are not allowed to spawn a Fire Station here (chance >= 25%).
						if (Tile->TileID == ForestTileID) {
							TileManager->ForestTiles.Remove(Tile);
						}
						if (Tile->TileID == GrassTileID) {
							TileManager->GrassTiles.Remove(Tile);
						}
						Tile->ApplyDataFromID(ResidentialTileID);
						TileManager->ResidentialTiles.Add(Tile);
					}
					else { // If we are allowed to spawn a Fire Station here (chance < 25%).
						if (!clusterFireStationPlaced) {
							if (Tile->TileID == ForestTileID) {
								TileManager->ForestTiles.Remove(Tile);
							}
							if (Tile->TileID == GrassTileID) {
								TileManager->GrassTiles.Remove(Tile);
							}
							Tile->ApplyDataFromID(FireStationID);
							clusterFireStationPlaced = true;
						}
					}
				}
			}
			fireStationTalley++; // Talley up how many times we've been through this loop...
			if (!clusterFireStationPlaced && fireStationTalley == Cluster.Num()) { // ...and if we are at the end of it...
				if (ATile* Tile = TileManager->TileLookup.FindRef(Coordinate)) {
					if (Tile->TileID != CommunicationsTowerID) {
						Tile->ApplyDataFromID(FireStationID); // ...create a Fire Station if one has not already been made yet.
						clusterFireStationPlaced = true;
					}
				}
			}
		}
	}

	// Lastly, get a random Grass Tile and turn it into a Water Tower.
	ATile* RandomGrassTile = TileManager->GrassTiles[FMath::RandRange(0, TileManager->GrassTiles.Num() - 1)];
	TileManager->GrassTiles.Remove(RandomGrassTile);
	RandomGrassTile->ApplyDataFromID(WaterTowerID);

	// Show how many generated Tiles there are.
	UE_LOG(LogTemp, Log, TEXT("Generated %d tiles."), ChosenTiles.Num());
}
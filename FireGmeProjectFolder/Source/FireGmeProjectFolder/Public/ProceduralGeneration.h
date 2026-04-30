#pragma once

#include "CoreMinimal.h"

class ATile;
class ATileManager;

class FIREGMEPROJECTFOLDER_API ProceduralGeneration
{
	public:
		// Generates a map for the game procedurally.
		static void GenerateMap(UWorld* World, TSubclassOf<ATile> TileClass, int32 NumberOfTiles,
								int32 GrassTileID, int32 ResidentialTileID, int32 ForestTileID, 
								int32 NonBurnableMountainTileID, int32 BurnableMountainTileID,
								int32 CommunicationsTowerID, int32 WaterTowerID,
								int32 FireStationID, ATileManager* TileManager);
};

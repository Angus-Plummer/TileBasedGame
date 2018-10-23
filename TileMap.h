// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "TileType.h"
#include "Unit.h"
#include "TileMap.generated.h"

// ---------- Tile Struct ---------- //
// struct for storing tile position and tile type. The data of the TileMap is composed of these structs

USTRUCT()
struct FTile
{
	GENERATED_USTRUCT_BODY()

public:

	FTile()
		: MapPosition(0,0,0)
		, TileTypeID(0)
	{}
	UPROPERTY(VisibleAnywhere)
	FIntVector MapPosition;
	UPROPERTY(VisibleAnywhere)
	int32 TileTypeID;
};

// equality operator for tile structs
inline bool operator==(const FTile& lhs, const FTile& rhs)
{
	// only need to compare the map positions as they are unique
	return lhs.MapPosition == rhs.MapPosition;
}

// type hash generator for tiles just uses the position as these are unique
inline int32 GetTypeHash(const FTile& Tile)
{
	return GetTypeHash(Tile.MapPosition);
}

// ---------- TileMap ---------- //

// Class used to manage tiles
UCLASS()
class ATileMap : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;


public:
	// ctor
	ATileMap();

	// ----------Map Geometry ---------- //

	// Max values of map coordinates in x,y,z directions
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	FIntVector MapSize;

	// Spacing of tiles
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	FVector TileSpacing;

	// ---------- Units ---------- //
	// TMap for determining if there is a unit at a given coordinate using Find(Coordinate). Can also find position of unit using FindKey(Unit) but this is a linear operation
	UPROPERTY(EditAnywhere)
	TMap<FIntVector, AUnit*> UnitPositions;

	// ---------- Tile Data ---------- //

	// Data Table containing the different tile types and their properties
	UPROPERTY(EditAnywhere)
	UDataTable* TileProperties;

	// 2d texture with map drawn on. this is used for creating and loading maps
	UPROPERTY(EditAnywhere)
	UTexture2D* SourceImage;

	// map container relating the tile coordinates to the tile structs. Use map instead of array so better handling of strange map shapes
	UPROPERTY()
	TMap<FIntVector, FTile> Tiles;
	// Need an instanced static mesh component for each tile type
	UPROPERTY()
	TArray<UInstancedStaticMeshComponent*> TileMeshes;

	// ---------- Highlighted Tiles ---------- //

	// currently focused tile
	UPROPERTY(EditAnywhere)
	FTile FocusedTile;
	// static mesh for the focused tile
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* FocusedTileMesh;
	UPROPERTY(EditAnywhere)
	UMaterialInstance* FocusedTileMaterial;
	
	// Set of highlighted tiles (for target of action other than for attack)
	UPROPERTY()
	TSet<FTile> MoveableTiles;
	// instanced static mesh for highlighted tiles 
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* MoveableTilesMesh;
	
	// Set of tiles highlighted for an attack action
	UPROPERTY()
	TSet<FTile> AttackableTiles;
	// instanced static mesh for tiles highlighted for attack
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* AttackableTilesMesh;

protected:
	// ---------- Begin AActor interface ---------- //

	// called after the constructor and when properties have been initialized
	// use this to set up the Instanced static mesh components as not everything works in the constructor
	virtual void PostInitProperties() override;

	// this runs when the map is constructed and any time any property is changed 
	// creates the tiles on first construction
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// this function runs after the properties of the object are changed in the editor
	// it recreates the tiles whenever the properties of the tile map are changed in the editor
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;

	// called when the object is to be destroyed
	// this function will destroy all the attached tile objects before the map is destroyed
	virtual void Destroyed() override;

	// ---------- End AActor interface ---------- //

private:
	bool bConstructed;

public:

	// adds a tile to the map at the given map coordinates. If there is already a tile at those coordinates it will delete and replace that tile
	void AddTile(int32 TileTypeID, FIntVector MapCoordinates);

	// creates the tiles according to the current height and width
	void CreateTiles();

	// clears the current tiles from the map
	void ClearMap();

	// gets the FTileType struct associated with a given tile type
	const FTileType* GetTypeData(int32 TileTypeID) const;

	// returns the map coordinate for a given world coordinate (x and y)
	FIntVector WorldToMapCoordinates(const FVector& WorldCoordinates) const;

	// returns the world coords of a map coordinate (centre of tile)
	FVector MapToWorldCoordinates(const FIntVector& MapCoordinates) const;

	// Set the tile a the given coordinates to be the focus tile. There can only be one focused tile
	void SelectFocusTile(FTile TileToFocus);

	// make so no tile is currently focused
	void UnsetFocusTile();

	// highlight a tile as moveable
	void AddMoveableTile(FTile Tile);

	// highlight a tile as attackable
	void AddAttackableTile(FTile Tile);

	// unhighlight all tiles (of either highlighted or attackable)
	void ClearHighlightedTiles();

	// get the manhattan distance between two map positions
	int32 DistanceBetween(const FIntVector MapPosition1, const FIntVector MapPosition2) const;

	// get all of the valid map positions that are adjacent to the input position including diagonal tiles
	TSet<FTile> GetSurroundingTiles(const FIntVector MapPosition) const;

	// returns a set containing the positions of tiles that are equal to or greater than the minimum distance and less than or equal to the maximum distance away from the source position 
	TSet<FTile> GetTilesInRange(const FIntVector SourcePosition, int32 MinimumDistance, int32 MaximumDistance) const;

	// adds a unit to the map at the given coordinates
	void AddUnit(AUnit* NewUnit, FIntVector MapPosition);

	// returns the map position of a unit
	FIntVector GetUnitPosition(AUnit* Unit) const;

	// returns a set of all units that are present on the set of tiles given
	TSet<AUnit*> GetUnitsOnTiles(TSet<FTile> Tiles);

	// return a set of tiles which can be reached by the input unit
	TSet<FTile> ReachableTiles(AUnit* UnitMoving) const;

	// return a sequence of coordinates that could be moved along to get from the starting coordinate to the target coordinate for a unit of particular team (units cannot move through enemy units but can move through allied ones)
	TArray<FIntVector> GetShortestPath(FIntVector StartCoordinate, FIntVector TargetCoordinate, int32 Team) const;

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
};




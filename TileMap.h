// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "TileType.h"
#include "Unit.h"
#include "TileMap.generated.h"

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
	FIntVector MapBounds;

	// Spacing of tiles
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	FVector TileSpacing;

	// ---------- Units ---------- //
	// TMap for determining if there is a unit at a given coordinate using Find(Coordinate). Can also find position of unit using FindKey(Unit) but this is a linear operation
	UPROPERTY(EditAnywhere)
	TMap<FIntVector, AUnit*> Units;

	// ---------- Tile Data ---------- //

	// Data Table containing the different tile types and their properties
	UPROPERTY(EditAnywhere)
	UDataTable* TileProperties;

	// 2d texture with map drawn on. this is used for creating and loading maps
	UPROPERTY(EditAnywhere)
	UTexture2D* SourceImage;

	// map container relating the tile coordinates to the tile types ids. Use map instead of array so better handling of strange map shapes
	UPROPERTY()
	TMap<FIntVector, int32> Tiles;
	// Need an instanced static mesh component for each tile type
	UPROPERTY()
	TArray<UInstancedStaticMeshComponent*> TileMeshes;

	// ---------- Highlighted Tiles ---------- //

	// location of the currently focused tile in map coordinates
	UPROPERTY(EditAnywhere)
	FIntVector FocusedTile;
	// static mesh for the focused tile
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* FocusedTileMesh;
	UPROPERTY(EditAnywhere)
	UMaterialInstance* FocusedTileMaterial;
	
	// Set holding the locations of the highlighted tiles (for target of action other than for attack)
	UPROPERTY()
	TSet<FIntVector> MoveableTiles;
	// instanced static mesh for highlighted tiles 
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* MoveableTilesMesh;
	
	// Set holding the locations of the tiles highlighted for an attack action
	UPROPERTY()
	TSet<FIntVector> AttackableTiles;
	// instanced static mesh for tiles highlighted for attack
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* AttackableTilesMesh;

protected:
	// ---------- Begin AActor interface ---------- //

	// called after the constructor and when properties have been initialised
	// use this to set up the Instanced static mesh components as not everything works in the constructor
	virtual void PostInitProperties() override;

	// this runs when the map is constructed and any time any property is changed 
	// creates the tiles on first construction
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// this function runs after the properties of the object are changed in the editor
	// it recreates the tiles whenever the properties of the tilemap are changed in the editor
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;

	// called when the object is to be destroyed
	// this function will destroy all the attached tile objects before the map is destroyed
	virtual void Destroyed() override;

	// ---------- End AActor interface ---------- //

private:
	bool bConstructed;

public:

	// adds a tile to the map at the given map coordinates. If there is already a tile at those coordinates it will delete and replace that tile
	void AddTile(const int32& TileTypeID, const FIntVector& MapCoordinates);

	// creates the tiles according to the current height and width
	void CreateTiles();

	// clears the current tiles from the map
	void ClearMap();

	// returns the integer type id of a tile at a given coordinates. If the coordinate is invalid, this will return -1.
	int32 GetTileType(const FIntVector& MapCoordinates) const;

	// gets the FTileType struct associated with a given tile type
	const FTileType* GetTypeData(int32 TileTypeID) const;

	// returns the map coordinate for a given world coordinate (x and y)
	FIntVector WorldToMapCoordinates(const FVector& WorldCoordinates) const;

	// returns the world coords of a map coordinate (centre of tile)
	FVector MapToWorldCoordinates(const FIntVector& MapCoordinates) const;

	// Set the tile a the given coordinates to be the focus tile. There can only be one focused tile
	void SelectFocusTile(const FIntVector MapCoordinates);

	// make so no tile is currently focused
	void UnsetFocusTile();

	// highlight a tile as moveable
	void AddMoveableTile(FIntVector MapPosition);

	// highlight a tile as attackable
	void AddAttackableTile(FIntVector MapPosition);

	// unhighlight all tiles (of either highlighted or attackable)
	void ClearHighlightedTiles();

	// get the manhattan distance between two map positions
	int32 DistanceBetween(const FIntVector MapPosition1, const FIntVector MapPosition2) const;

	// get all of the valid map positions that are adjacent to the input position (checks will not return a position if it is not in TMap)
	// currently only gives the 2D adjacencies (in x and y)
	TSet<FIntVector> GetAdjacentTiles(const FIntVector MapPosition);


	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
};




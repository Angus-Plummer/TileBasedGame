// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
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
	ATileMap();

	// Number of tiles along each side of grid
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	int32 Width;
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	int32 Height;
	
	// map container relating the different tile types to their blueprint class
	UPROPERTY(EditAnywhere)
	TMap<ETileType, TSubclassOf<ATile>> TileTypes;
	// map container relating the tile coordinates to the actual tiles. Use map instead of array so better handling of strange map shapes
	UPROPERTY()
	TMap<FIntVector, ATile*> Tiles;

	// Spacing of tiles
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float TileSpacing;

protected:
	// Begin AActor interface

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// this is used to initiate all of the tiles and attach them to the map
	virtual void OnConstruction(const FTransform& Transform) override;

	// this function runs after the properties of the object are changed in the editor
	// it recreates the tiles whenever the properties of the tilemap are changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& change_event) override;

	// called when the object is to be destroyed
	// this function will destroy all the attached tile objects before the map is destroyed
	virtual void Destroyed() override;

	// End AActor interface

public:

	// adds a tile to the map at the given map coordinates. If there is already a tile at those coordinates it will delete and replace that tile
	void AddTile(const ETileType& NewTileType, const FIntVector& MapCoordinates);

	// removes the tile at the given map coordinates
	void RemoveTile(const FIntVector& MapCoordinates);

	// creates the tiles according to the current height and width
	void CreateTiles();

	// clears the current tiles from the map
	void ClearMap();

	// returns a raw pointer to the tile at a given map coordinate
	ATile* GetTile(const FIntVector& MapCoordinates) const;

	// returns the map coordinate for a given world coordinate (x and y)
	FIntVector WorldToMapCoordinates(const FVector& WorldCoordinates) const;

	// returns the world coords of a map coordinate (centre of tile)
	FVector MapToWorldCoordinates(const FIntVector& MapCoordinates) const;

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
};




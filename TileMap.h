// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "TileData.h"
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

	// Need an instanced static mesh component for each tile type so use a map for quick lookup by tile type
	UPROPERTY(EditAnywhere)
	//TMap<ETileType, UInstancedStaticMeshComponent*> TileMesh;
	UInstancedStaticMeshComponent* TileMesh;

	UPROPERTY(EditAnywhere)
	UDataTable* TileProperties;
	//FDataTableRowHandle TileProperties;

	// map container relating the tile coordinates to the actual tiles. Use map instead of array so better handling of strange map shapes
	UPROPERTY(EditAnywhere)
	TMap<FIntVector, ETileType> Tiles;

	// Spacing of tiles
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float TileSpacing;

protected:
	// Begin AActor interface

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// this runs when the map is constructd and any time any property is changed 
	// it is used to initiate all of the tiles and attach them to the map
	virtual void OnConstruction(const FTransform& Transform) override;

	// this function runs after the properties of the object are changed in the editor
	// it recreates the tiles whenever the properties of the tilemap are changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& change_event) override;

	// called when the object is to be destroyed
	// this function will destroy all the attached tile objects before the map is destroyed
	virtual void Destroyed() override;

	// End AActor interface

private:
	bool bConstructed;

public:

	// adds a tile to the map at the given map coordinates. If there is already a tile at those coordinates it will delete and replace that tile
	void AddTile(const ETileType& NewTileType, const FIntVector& MapCoordinates);

	// creates the tiles according to the current height and width
	void CreateTiles();

	// clears the current tiles from the map
	void ClearMap();

	// returns a raw pointer to the tile at a given map coordinate
	ETileType GetTileType(const FIntVector& MapCoordinates) const;

	// gets the data struct associated with a given tile type
	const FTileData* GetTypeData(ETileType TileType) const;

	// returns the map coordinate for a given world coordinate (x and y)
	FIntVector WorldToMapCoordinates(const FVector& WorldCoordinates) const;

	// returns the world coords of a map coordinate (centre of tile)
	FVector MapToWorldCoordinates(const FIntVector& MapCoordinates) const;

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
};




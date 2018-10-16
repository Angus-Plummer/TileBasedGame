// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TileMap.h"
#include "Tile.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

ATileMap::ATileMap()
{
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Set defaults
	Width = 3;
	Height = 3;
	TileSpacing = 300.f;
}


void ATileMap::BeginPlay()
{
	Super::BeginPlay();

	//CreateTiles();
}

// sets the tiles
void ATileMap::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// if the map is empty then create the map
	if (Tiles.Num() == 0) // tile actors with own meshes
	{
		CreateTiles();
	}
}

// recreates the tiles whenever the properties of the tile map are changed in the editor
void ATileMap::PostEditChangeProperty(struct FPropertyChangedEvent& ChangedProperty)
{
	Super::PostEditChangeProperty(ChangedProperty);

	// get name of changed property
	FName property_name = ChangedProperty.GetPropertyName();

	// if the changed property is any of these map member variables then recreate the map
	if (property_name == GET_MEMBER_NAME_CHECKED(ATileMap, Width)
		|| property_name == GET_MEMBER_NAME_CHECKED(ATileMap, Height)
		|| property_name == GET_MEMBER_NAME_CHECKED(ATileMap, TileSpacing))
	{
		CreateTiles();
	}
}

void ATileMap::Destroyed()
{
	// clear the map of all tiles before continuing
	ClearMap();
	Super::Destroyed();
}



void ATileMap::AddTile(const ETileType& NewTileType, const FIntVector& MapPosition)
{

	// if the new tile is of a valid type then add it
	if (TileTypes.Contains(NewTileType))
	{
		// convert the position from map to world coordinates
		FVector WorldPosition = MapToWorldCoordinates(MapPosition);

		// get the blueprint class from the tiletypes map
		TSubclassOf<ATile> BlueprintTile = TileTypes[NewTileType];

		// set up remaining spawn parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		// spawn the tile
		ATile* NewTile = GetWorld()->SpawnActor<ATile>(BlueprintTile, WorldPosition, FRotator(0, 0, 0), SpawnParams);

		// if the new tile has spawned correctly then continue with creation
		if (NewTile)
		{
			// label the tile so it is easier to identify in the editor
			FString TileLabel = ("Tile[" + FString::FromInt(MapPosition.X) + "][" + FString::FromInt(MapPosition.Y) + "]"); // label is Tile[X][Y]
			NewTile->SetActorLabel(TileLabel);

			// attach the new tile to this map
			NewTile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			NewTile->OwningMap = this;

			// if there is already a tile on the map at the new tile position then first remove that tile
			if (Tiles.Contains(MapPosition))
			{
				RemoveTile(MapPosition);
			}
			// now add the new tile to the map
			Tiles.Add(MapPosition, NewTile);
		}
	}
}

void ATileMap::RemoveTile(const FIntVector& MapPosition)
{
	// if the Tiles TMap contains a tile at the given position then destroy it and free the corresponding key in the TMap
	if (Tiles.Contains(MapPosition))
	{
		Tiles[MapPosition]->Destroy();
		Tiles.Remove(MapPosition);
	}
}

void ATileMap::CreateTiles()
{
	// first clear the current map
	ClearMap();

	// only spawn if world exists
	if (GetWorld())
	{
		for (int32 j = 0; j < Height; j++)
		{
			for (int32 i = 0; i < Width; i++)
			{
				// get the map position of the tile
				FIntVector MapPosition(i, j, 0);
				// FOR NOW FILL WITH GRASS TILES
				ETileType NewTileType = ETileType::GRASS;
				// add the new tile to the map
				AddTile(NewTileType, MapPosition);
			}
		}
	}
}


void ATileMap::ClearMap()
{
	// destroy all tiles in the Tiles TMap
	for (auto Elem : Tiles)
	{
		Elem.Value->Destroy();
	}

	// empty the Tiles TMap of all pairs
	Tiles.Empty();
}

ATile* ATileMap::GetTile(const FIntVector& MapPosition) const
{
	return Tiles[MapPosition];
}

FIntVector ATileMap::WorldToMapCoordinates(const FVector& WorldPosition) const
{
	// tiles are centered at their world coordinates so must first correct for this offset
	float CorrectedWorldX = WorldPosition.X + 0.5 * TileSpacing;
	float CorrectedWorldY = WorldPosition.Y + 0.5 * TileSpacing;

	// get the map coords using the tile size, ignoring the remainder
	int MapX = CorrectedWorldX / TileSpacing;
	int MapY = CorrectedWorldY / TileSpacing;

	return FIntVector(MapX, MapY, 0);
}

FVector ATileMap::MapToWorldCoordinates(const FIntVector& MapPosition) const
{
	// scale up to world scale
	float ScaledX = (float)MapPosition.X * TileSpacing;
	float ScaledY = (float)MapPosition.Y * TileSpacing;

	// correct for position of this in world space
	FVector WorldPosition = GetActorLocation() + FVector(ScaledX, ScaledY, 0);

	return WorldPosition;
}

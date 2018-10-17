// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TileMap.h"
#include "Tile.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

ATileMap::ATileMap() 
	: bConstructed(false)
{
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	ConstructorHelpers::FObjectFinderOptional<UDataTable> TilePropertiesP(TEXT("/Game/TileMap/TileProperties.TileProperties"));
	TileProperties = TilePropertiesP.Get();

	// create tile mesh
	TileMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Tile Mesh"));

	// set up the mesh
	const FTileData* GrassTileData = GetTypeData(ETileType::GRASS);
	TileMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	TileMesh->SetupAttachment(DummyRoot);
	//TileMesh->SetStaticMesh(GrassTileData->Mesh);
	//TileMesh->SetMaterial(0, GrassTileData->Material);

	// Set defaults
	Width = 3;
	Height = 3;
	TileSpacing = 250.f;
}


void ATileMap::BeginPlay()
{
	Super::BeginPlay();
}

// sets the tiles
void ATileMap::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// if the map has not yet been constructed before then do this..
	// this code will only run one time, when the contructor is first constructed
	if (!bConstructed)
	{
		bConstructed = true;
		// set up the mesh
		const FTileData* GrassTileData = GetTypeData(ETileType::GRASS);
		TileMesh->SetMaterial(0, GrassTileData->Material);
		TileMesh->SetStaticMesh(GrassTileData->Mesh);
		// create the tiles
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
	if (true)//TileTypes.Contains(NewTileType)) 
	{
		// add new tile data to the tiles TMap
		Tiles.Add(MapPosition, NewTileType);

		FTransform NewTileTransform;
		NewTileTransform.SetLocation(MapToWorldCoordinates(MapPosition));

		// Add the tile to its correspondin instanced mesh
		TileMesh->AddInstanceWorldSpace(NewTileTransform);
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
	// empty the Tiles TMap of all pairs
	Tiles.Empty();
	
	// clear all instances from the mesh
	TileMesh->ClearInstances();
}

ETileType ATileMap::GetTileType(const FIntVector& MapPosition) const
{
	return Tiles[MapPosition];
}

const FTileData* ATileMap::GetTypeData(ETileType TileType) const
{
	// first convert the FTileType enum into an FName of its int8 value
	FName TileID = (FName)*FString::FromInt((int8)TileType);
	// find the tile by its ID in the TileProperties data table
	FTileData* TileData = TileProperties->FindRow<FTileData>(TileID, FString(""));
	return TileData;
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

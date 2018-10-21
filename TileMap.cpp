// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TileMap.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"

ATileMap::ATileMap() 
	: bConstructed(false)
{
	// Set defaults
	MapBounds = FIntVector(3, 3, 3);
	TileSpacing = FVector(250.f, 250.f, 25.f);

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// set up pointer to the tile properties data table
	ConstructorHelpers::FObjectFinderOptional<UDataTable> TilePropertiesP(TEXT("/Game/TileMap/TileProperties.TileProperties"));
	TileProperties = TilePropertiesP.Get();

	// ---------- create meshes for the focused and highlighted tiles ---------- //

	// set up mesh components. Choice of mesh and materials must be done in editor
	FocusedTileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Focused Tile Mesh"));
	FocusedTileMesh->SetRelativeScale3D(FVector(1.f,1.f,0.1f));
	FocusedTileMesh->SetRelativeLocation(FVector(0.f,0.f,0.f));
	FocusedTileMesh->SetupAttachment(DummyRoot);
	
	MoveableTilesMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Moveable Tiles Mesh"));
	MoveableTilesMesh->SetRelativeScale3D(FVector(1.f, 1.f, 0.1f));
	MoveableTilesMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	MoveableTilesMesh->SetupAttachment(DummyRoot);

	AttackableTilesMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Attackable Tiles Mesh"));
	AttackableTilesMesh->SetRelativeScale3D(FVector(1.f, 1.f, 0.1f));
	AttackableTilesMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	AttackableTilesMesh->SetupAttachment(DummyRoot);
}

void ATileMap::PostInitProperties()
{
	Super::PostInitProperties();

	// set up the mesh
// 	TileMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
// 	TileMesh->SetupAttachment(DummyRoot);

	
}

// creates the tiles on first construction
void ATileMap::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// if the map has not yet been constructed before then do this..
	// this code will only run one time, when the contructor is first constructed

	if (!bConstructed)
	{
		bConstructed = true;

		// first clear the array of pointers to the meshes if not already empty
		TileMeshes.Empty();

		// create the instanced meshes. 1 for each tile type in the data table
		TArray<FTileType*> TileTypes;
		TileProperties->GetAllRows(FString(""), TileTypes);

		for (auto TileTypeData : TileTypes)
		{
			FName NameComponent = FName(*(FString("StaticMesh_") + FString::FromInt(TileTypeData->ID)));
			UInstancedStaticMeshComponent* NewMesh = NewObject<UInstancedStaticMeshComponent>(this, NameComponent);
			if (NewMesh)
			{
				NewMesh->RegisterComponent();
				NewMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
				NewMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
				NewMesh->SetStaticMesh(TileTypeData->Mesh);
				NewMesh->SetMaterial(0, TileTypeData->Material);
				TileMeshes.Add(NewMesh);
			}
		}

		// create the tiles
		CreateTiles();
	}
}


void ATileMap::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = MapBounds.X / 3; i < MapBounds.X * 2 / 3; i++)
	{
		for (int32 j = MapBounds.Y / 3; j < MapBounds.Y * 2 / 3; j++)
		{
			AddMoveableTile(FIntVector(i, j, 0));
		}
	}
	for (auto Tile : MoveableTiles)
	{
		TSet<FIntVector> AdjacentTiles = GetAdjacentTiles(Tile);
		for (auto TargetTile : AdjacentTiles)
		{
			if (!MoveableTiles.Contains(TargetTile))
			{
				AddAttackableTile(TargetTile);
			}
		}

	}
	
	
}

// recreates the tiles whenever the properties of the tile map are changed in the editor
void ATileMap::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// get the member variable that was changed
	UProperty* Property = PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue();
	if (Property) {
		// get name of changed property
		FName PropertyName = Property->GetFName();
		// if the changed property is any of these map member variables then recreate the map
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ATileMap, MapBounds)
			|| PropertyName == GET_MEMBER_NAME_CHECKED(ATileMap, TileSpacing))
		{
			CreateTiles();
		}
	}
}

void ATileMap::Destroyed()
{
	// clear the map of all tiles before continuing
	ClearMap();
	Super::Destroyed();
}


void ATileMap::AddTile(const int32& TileTypeID, const FIntVector& MapPosition)
{
	// add new tile data to the tiles TMap
	Tiles.Add(MapPosition, TileTypeID);

	FTransform NewTileTransform;
	NewTileTransform.SetLocation(FVector(MapPosition) * TileSpacing);

	// Add the tile to its corresponding instanced mesh
	TileMeshes[TileTypeID]->AddInstance(NewTileTransform);
}

void ATileMap::CreateTiles()
{
	// first clear the current map
	ClearMap();

	// only spawn if world exists
	if (GetWorld())
	{
		TMap<FString, int32> ColorToTileID; // map relating colors (as FString Hexadecimal RGBA) representing tiles on the map to the tile IDs
		// get array holding all tile type data in the TileProperties data file
		TArray<FTileType*> TileTypeData;
		TileProperties->GetAllRows(FString(""), TileTypeData);

		for (auto TileType : TileTypeData)
		{
			ColorToTileID.Add(TileType->SourceImageColour.ToHex(), TileType->ID);
		}

		// if there is a map source image then load from the image, otherwise load no tiles
		if (SourceImage)
		{
			// set the map bounds to be those of the source image
			MapBounds = FIntVector(SourceImage->GetSizeX(), SourceImage->GetSizeY(), 1);

			// set up the source image settings so that it allows finding rgb pixel colours
			SourceImage->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
			SourceImage->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
			SourceImage->SRGB = false;
			SourceImage->UpdateResource();

			// get pixel color array from the texture
			const FColor* FormatedImageData = static_cast<const FColor*>(SourceImage->PlatformData->Mips[0].BulkData.LockReadOnly());
			// iterate through the array and get each colour
			for (int32 X = 0; X < SourceImage->GetSizeX(); X++)
			{
				for (int32 Y = 0; Y < SourceImage->GetSizeY(); Y++)
				{
					FString PixelColor = FormatedImageData[Y * SourceImage->GetSizeX() + X].ToHex();					
					// if the colour of the pixel matches with a tile type from the data table then add a tile of that type to the map at the matching coordinate
					if (ColorToTileID.Contains(PixelColor))
					{
						AddTile(ColorToTileID[PixelColor], FIntVector(X, Y, 0));
					}
				}
			}
			// unlock the source image so it can be edited elsewhere
			SourceImage->PlatformData->Mips[0].BulkData.Unlock();
		}
	}
}


void ATileMap::ClearMap()
{
	// empty the Tiles TMap of all pairs
	Tiles.Empty();
	
	// clear all instances from the mesh
	for (auto TileMesh : TileMeshes)
	{
		if (TileMesh)
		{
			TileMesh->ClearInstances();
		}
	}
}

int32 ATileMap::GetTileType(const FIntVector& MapPosition) const
{	
	const int32* TypeID = Tiles.Find(MapPosition);
	if (TypeID)
	{
		return *TypeID;
	}
	else
		return -1;
}

const FTileType* ATileMap::GetTypeData(int32 TileTypeID) const
{
	// first convert the FTileType enum into an FName of its int32 value
	FName TileID = (FName)*FString::FromInt(TileTypeID);
	// find the tile by its ID in the TileProperties data table
	FTileType* TileData = TileProperties->FindRow<FTileType>(TileID, FString(""));
	return TileData;
}

FIntVector ATileMap::WorldToMapCoordinates(const FVector& WorldPosition) const
{
	// first translate from the world coordinates to the coordinates of the tilemap actor
	FVector LocalPosition = this->GetTransform().InverseTransformPosition(WorldPosition);
	
	// divide by tile spacing to get to map scale and correct for offset due to tiles coordinates being at their centre
	FVector MapPosition = LocalPosition / TileSpacing + FVector(0.5, 0.5, 0.5);

	// if map is 2d (bound of map in z is 1) then set the map position in z to be 0 (project down z axis to find tile)
	if (MapBounds.Z == 1) 
	{
		MapPosition.Z = 0;
	}
	
	// convert to int vector and return
	return FIntVector(MapPosition);
}

FVector ATileMap::MapToWorldCoordinates(const FIntVector& MapPosition) const
{
	// scale up to local scale
	FVector LocalPosition = FVector(MapPosition) * TileSpacing;

	// convert position to world coordinates
	FVector WorldPosition = GetTransform().TransformPosition(LocalPosition);

	return WorldPosition;
}

void ATileMap::SelectFocusTile(const FIntVector MapCoordinates)
{
	FocusedTile = MapCoordinates;
	if (!FocusedTileMesh->IsVisible())
	{
		FocusedTileMesh->SetVisibility(true);
	}
	FocusedTileMesh->SetRelativeLocation(FVector(MapCoordinates) * TileSpacing);
}

void ATileMap::UnsetFocusTile()
{
	if (FocusedTileMesh->IsVisible())
	{
		FocusedTileMesh->SetVisibility(false);
		FocusedTile = FIntVector(NULL, NULL, NULL);
	}
}

void ATileMap::AddMoveableTile(FIntVector MapPosition)
{
	// check if it has been added already
	if (!MoveableTiles.Contains(MapPosition))
	{
		// add the tile to the movable tiles set 
		MoveableTiles.Add(MapPosition);
		// add a new instanced static mesh component to the movable tiles mesh
		FTransform NewTileTransform;
		NewTileTransform.SetLocation(FVector(MapPosition) * TileSpacing);
		MoveableTilesMesh->AddInstance(NewTileTransform);
	}
}

void ATileMap::AddAttackableTile(FIntVector MapPosition)
{
	// check if the tile has already been added to attackable tiles
	if (!AttackableTiles.Contains(MapPosition))
	{
		// add the tile to the movable tiles set (will do nothing if it is already in the set as it is a TSet)
		AttackableTiles.Add(MapPosition);
		// add a new instanced static mesh component to the movable tiles mesh
		FTransform NewTileTransform;
		NewTileTransform.SetLocation(FVector(MapPosition) * TileSpacing);
		AttackableTilesMesh->AddInstance(NewTileTransform);
	}
}

void ATileMap::ClearHighlightedTiles()
{
	// empty the TArrays holding the highlighted tile positions
	AttackableTiles.Empty();
	MoveableTiles.Empty();

	// clear the instanced static meshes
	AttackableTilesMesh->ClearInstances();
	MoveableTilesMesh->ClearInstances();
}

int32 ATileMap::DistanceBetween(const FIntVector MapPosition1, const FIntVector MapPosition2) const
{
	FIntVector Delta = MapPosition2 - MapPosition1;
	int32 ManhattanDistance = FMath::Abs(Delta.X) + FMath::Abs(Delta.Y) + FMath::Abs(Delta.Z);
	return ManhattanDistance;
}

TSet<FIntVector> ATileMap::GetAdjacentTiles(const FIntVector MapPosition)
{
	TSet<FIntVector> AdjacentTiles;
	// iterate through the tiles at positions around the map position
	for (int32 i = -1; i <= 1; i++)
	{
		for (int32 j = -1; j <= 1; j++)
		{
			// check if the map position is in the Tiles TMap and add it to the AdjacentTiles array if it is
			// exclude i==j==0 as that is just the starting map position 
			if ( !(i == 0 && j == 0) && Tiles.Contains( MapPosition + FIntVector(i,j,0) ) )
			{
				AdjacentTiles.Add(MapPosition + FIntVector(i, j, 0));
			}
		}
	}
	return AdjacentTiles;
}

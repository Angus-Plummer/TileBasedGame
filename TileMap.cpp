// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TileMap.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "MoveSequence.h"

ATileMap::ATileMap() 
	: bConstructed(false)
{
	// Set defaults
	MapSize = FIntVector(3, 3, 3);
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
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ATileMap, MapSize)
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


void ATileMap::AddTile(int32 TileTypeID, FIntVector MapCoordinates)
{
	FTile NewTile;
	NewTile.TileTypeID = TileTypeID;
	NewTile.MapPosition = MapCoordinates;

	// add new tile data to the tiles TMap
	Tiles.Add(NewTile.MapPosition, NewTile);

	FTransform NewTileTransform;
	NewTileTransform.SetLocation(FVector(NewTile.MapPosition) * TileSpacing);

	// Add the tile to its corresponding instanced mesh
	TileMeshes[NewTile.TileTypeID]->AddInstance(NewTileTransform);
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
			MapSize = FIntVector(SourceImage->GetSizeX(), SourceImage->GetSizeY(), 1);

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
						AddTile( *ColorToTileID.Find(PixelColor), FIntVector(X, Y, 0));
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
	if (MapSize.Z == 1) 
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

void ATileMap::SelectFocusTile(FTile Tile)
{
	FocusedTile = Tile;
	if (!FocusedTileMesh->IsVisible())
	{
		FocusedTileMesh->SetVisibility(true);
	}
	FocusedTileMesh->SetRelativeLocation(FVector(FocusedTile.MapPosition) * TileSpacing);
}

void ATileMap::UnsetFocusTile()
{
	if (FocusedTileMesh->IsVisible())
	{
		FocusedTileMesh->SetVisibility(false);
		FocusedTile = FTile();
	}
}

void ATileMap::AddMoveableTile(FTile Tile)
{
	// check if it has been added already
	if (!MoveableTiles.Contains(Tile))
	{
		// add the tile to the movable tiles set 
		MoveableTiles.Add(Tile);
		// add a new instanced static mesh component to the movable tiles mesh
		FTransform NewTileTransform;
		NewTileTransform.SetLocation(FVector(Tile.MapPosition) * TileSpacing);
		MoveableTilesMesh->AddInstance(NewTileTransform);
	}
}

void ATileMap::AddAttackableTile(FTile Tile)
{
	// check if the tile has already been added to attackable tiles
	if (!AttackableTiles.Contains(Tile))
	{
		// add the tile to the movable tiles set (will do nothing if it is already in the set as it is a TSet)
		AttackableTiles.Add(Tile);
		// add a new instanced static mesh component to the movable tiles mesh
		FTransform NewTileTransform;
		NewTileTransform.SetLocation(FVector(Tile.MapPosition) * TileSpacing);
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

TSet<FTile> ATileMap::GetSurroundingTiles(const FIntVector MapPosition) const
{
	TSet<FTile> AdjacentTiles;
	// iterate through the tiles at positions around the map position
	for (int32 i = -1; i <= 1; i++)
	{
		for (int32 j = -1; j <= 1; j++)
		{
			// check if the map position is in the Tiles TMap and add it to the AdjacentTiles array if it is
			// exclude i==j==0 as that is just the starting map position 
			if ( !(i == 0 && j == 0) && Tiles.Contains( MapPosition + FIntVector(i,j,0) ) )
			{
				AdjacentTiles.Add(*Tiles.Find(MapPosition + FIntVector(i, j, 0)));
			}
		}
	}
	return AdjacentTiles;
}

TSet<FTile> ATileMap::GetTilesInRange(const FIntVector SourcePosition, int32 MinimumDistance, int32 MaximumDistance) const
{
	TSet<FTile> TilesInRange;
	// iterate through the tile positions in a square of side 2 * max distance centred on the source position
	for (int32 i = -MaximumDistance; i <= MaximumDistance; i++)
	{
		for (int32 j = -MaximumDistance; j <= MaximumDistance; j++)
		{
			int32 ManhattanDistance = FMath::Abs(i) + FMath::Abs(j);
			// check whether the distance is within the desired range
			if (ManhattanDistance >= MinimumDistance && ManhattanDistance <= MaximumDistance)
			{
			// check if the map position is in the Tiles TMap and add a pointer to the corresponding tile to the TilesInRange set if it is
				if (Tiles.Contains(SourcePosition + FIntVector(i, j, 0)))
				{
					TilesInRange.Add(*Tiles.Find(SourcePosition + FIntVector(i, j, 0)));
				}
			}
			
		}
	}
	return TilesInRange;
}

void ATileMap::AddUnit(AUnit* NewUnit, FIntVector MapPosition)
{
	if (Tiles.Contains(MapPosition))
	{
		UnitPositions.Add(MapPosition, NewUnit);
	}
}

FIntVector ATileMap::GetUnitPosition(AUnit* Unit) const
{
	const FIntVector* UnitPosition = UnitPositions.FindKey(Unit);
	if (UnitPosition)
	{
		return *UnitPosition;
	}
	return FIntVector(0, 0, 0);
}

TSet<AUnit*> ATileMap::GetUnitsOnTiles(TSet<FTile> Tiles)
{
	TSet<AUnit*> UnitsFound;
	for (auto Tile : Tiles)
	{
		if (UnitPositions.Contains(Tile.MapPosition))
		{
			UnitsFound.Add(*UnitPositions.Find(Tile.MapPosition));
		}
	}
	return UnitsFound;
}

TSet<FTile> ATileMap::ReachableTiles(AUnit* UnitMoving) const
{

}

TArray<FIntVector> ATileMap::GetShortestPath(FIntVector StartCoordinate, FIntVector TargetCoordinate, int32 Team) const
{
	// doubly linked lists used for storing the move sequences as each element must maintain its memory address when elements are added as they are pointed to by other move sequences
	TDoubleLinkedList<MoveSequence> OpenSet; // tiles that will potentially be analysed
	TDoubleLinkedList<MoveSequence> ClosedSet; // tiles that have been analysed or are being analysed
	
	// create the starting and final move sequence elements. Start is the at the units position, the end is the target
	MoveSequence StartTile(StartCoordinate, GetTypeData(Tiles.Find(StartCoordinate)->TileTypeID)->MoveCost);
	MoveSequence EndTile(TargetCoordinate, GetTypeData(Tiles.Find(TargetCoordinate)->TileTypeID)->MoveCost);
	
	// add the initial tile to the open set
	StartTile.Target(TargetCoordinate);
	OpenSet.AddHead(StartTile);

	// keep iterating over this loop until EndTile has a parent, at that point the end tile must have been reached!
	while (!EndTile.GetSequenceParent()) {
		
		// find the tile in the open set with the lowest predicted score to the target
		MoveSequence* ElemToAnalyse = &OpenSet.GetHead()->GetValue();
		for (auto& Elem : OpenSet)
		{
			if (ElemToAnalyse == &StartTile)
			{
				ElemToAnalyse = &Elem;
			}
			else
			{
				if (Elem.Score() < ElemToAnalyse->Score())
				{
					ElemToAnalyse = &Elem;
				}
			}
		}

		// add a copy of the element that will be analysed to the closed set
		ClosedSet.AddHead(*ElemToAnalyse);
		// remove the element to analyse from the open set as it cannot be in both sets and update the pointer to the closed set copy
		MoveSequence* ElemToAnalyseOpenSet = ElemToAnalyse;
		ElemToAnalyse = &ClosedSet.FindNode(*ElemToAnalyse)->GetValue();
		OpenSet.RemoveNode(*ElemToAnalyseOpenSet);
		ElemToAnalyseOpenSet = nullptr;

		// set to hold tiles adjacent to inspected tile
		TSet<FTile> AdjacentTiles = GetTilesInRange(ElemToAnalyse->GetMapCoordinates(), 1, 1); // tiles distance of 1 map unit away
		// go through the tiles in the set of adjacent tiles
		for (auto Tile : AdjacentTiles) {
			
			// make a MoveSequence object from this adjacent tile and make its parent the currently inspected tile
			MoveSequence AdjacentElement(Tile.MapPosition, GetTypeData(Tile.TileTypeID)->MoveCost);
			AdjacentElement.SetSequenceParent(*ElemToAnalyse);
			AdjacentElement.Target(EndTile.GetMapCoordinates());
			
			// check if this element is at the target position. if so update the end tile and we have found the shortest path
			if (AdjacentElement.GetMapCoordinates() == EndTile.GetMapCoordinates()) {
				EndTile = AdjacentElement;
			}
			
			// check if the element is already in one of the sets (elements compared using their map coordinates)
			MoveSequence* InOpenSet = &OpenSet.FindNode(AdjacentElement)->GetValue();
			MoveSequence* InClosedSet = &ClosedSet.FindNode(AdjacentElement)->GetValue();
			// if this tile is not already in the closed set then proceed (if it is in closed set then it has already been analysed and shortest path to it found)
			if (!InClosedSet) {
				if (!InOpenSet) { // if also not in open set
					// if there is a unit on the tile..
					if (UnitPositions.Contains(AdjacentElement.GetMapCoordinates())) {
						// friendly unit -> can move through, non-friendly then cant
							if (Team == ((*UnitPositions.Find(AdjacentElement.GetMapCoordinates()))->GetTeam())) {
							OpenSet.AddHead(AdjacentElement);
						}
					}
					// if no unit on the tile then just add to reachable tiles and to open set
					else {
						OpenSet.AddHead(AdjacentElement);
					}
				}
				else { // (in open set already)
					// if adjacent tile costs less to reach from parent as currently inspected element then update the parent of element already in set
					if (AdjacentElement.Score() < InOpenSet->Score()) {
						InOpenSet->SetSequenceParent(*ElemToAnalyse);
					}
				}
			}
		}
	}
	// we now have a move sequence which takes the unit from its current tile to the target tile.
	// make list of coordinates that is the order of moves to reach the target using the MoveSequence corresponding to the fnial tile
	TDoubleLinkedList<FIntVector> ShortestPathList;;
	MoveSequence CurrentElement = EndTile;
	while (CurrentElement.GetSequenceParent()) { // will keep adding until we reach then initial tile has not parent and is not added
		ShortestPathList.AddHead(CurrentElement.GetMapCoordinates());
		CurrentElement = *CurrentElement.GetSequenceParent();
	}
	// convert list to array
	TArray<FIntVector> ShortestPathArray;
	for (auto elem : ShortestPathList)
	{
		ShortestPathArray.Add(elem);
	}
	return ShortestPathArray;
}

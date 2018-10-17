// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Tile.h"
#include "TileMap.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"
// 
// ATile::ATile()
// {
// 	Create dummy root scene component
// 	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
// 	RootComponent = DummyRoot;
// 
// 	ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> DefaultMaterial(TEXT("/Game/TileMap/Meshes/DefaultMaterial.DefaultMaterial"));
// 	ConstructorHelpers::FObjectFinderOptional<UStaticMesh> SquareTileMesh(TEXT("/Game/TileMap/Meshes/SquareTileMesh.SquareTileMesh"));
// 	ConstructorHelpers::FObjectFinderOptional<UStaticMesh> HexTileMesh1(TEXT("/Game/TileMap/Meshes/HexTileMesh1.HexTileMesh1"));
// 	ConstructorHelpers::FObjectFinderOptional<UStaticMesh> HexTileMesh2(TEXT("/Game/TileMap/Meshes/HexTileMesh2.HexTileMesh2"));
// 	Material = DefaultMaterial.Get();
// 	Mesh = SquareTileMesh.Get();
// 
// 	// Create static mesh component
// 	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
// 	TileMesh->SetStaticMesh(Mesh);
// 	TileMesh->SetRelativeScale3D(FVector(1.f,1.f,0.1f));
// 	TileMesh->SetRelativeLocation(FVector(0.f,0.f,0.f));
// 	TileMesh->SetMaterial(0, Material);
// 	TileMesh->SetupAttachment(DummyRoot);
// 	TileMesh->OnClicked.AddDynamic(this, &ATile::TileClicked);
// 	TileMesh->OnInputTouchBegin.AddDynamic(this, &ATile::OnFingerPressedTile);
// 
// 	TileType = ETileType::BLANK;
// 	MoveCost = 1;
// 	DefenseModifier = 0;
// 	AttackModifier = 0;
// }
// 
// void ATile::PostInitProperties()
// {
// 	Super::PostInitProperties();
// 	
// 	UpdateMesh();
// }
// 
// void ATile::PostEditChangeProperty(struct FPropertyChangedEvent& ChangedProperty)
// {
// 	Super::PostEditChangeProperty(ChangedProperty);
// 
// 	// get name of changed property
// 	FName property_name = ChangedProperty.GetPropertyName();
// 
// 	// if the changed property is any of these map member variables then recreate the map
// 	if (property_name == GET_MEMBER_NAME_CHECKED(ATile, Mesh)
// 		|| property_name == GET_MEMBER_NAME_CHECKED(ATile, Material))
// 	{
// 		UpdateMesh();
// 	}
// }
// 
// void ATile::UpdateMesh()
// {
// 	// update visual properties
// 	if (TileMesh)
// 	{
// 		TileMesh->SetStaticMesh(Mesh);
// 		TileMesh->SetMaterial(0, Material);
// 	}
// }
// 
// void ATile::TileClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked)
// {
// 	HandleClicked();
// }
// 
// 
// void ATile::OnFingerPressedTile(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
// {
// 	HandleClicked();
// }
// 
// void ATile::HandleClicked()
// {
// 	// Check we are not already active
// 	if (!bIsActive)
// 	{
// 		bIsActive = true;
// 
// 		// Change material
// 		//TileMesh->SetMaterial(0, Material);
// 
// 		// Tell the Grid
// 		//if (OwningMap != nullptr)
// 		//{
// 		//	//OwningMap->AddScore();
// 		//}
// 	}
// }
// 
// void ATile::Highlight(bool bOn)
// {
// 	// Do not highlight if the tile has already been activated.
// 	if (bIsActive)
// 	{
// 		return;
// 	}
// 
// 	if (bOn)
// 	{
// 		//TileMesh->SetMaterial(0, Material);
// 	}
// 	else
// 	{
// 		//TileMesh->SetMaterial(0, Material);
// 	}
// }

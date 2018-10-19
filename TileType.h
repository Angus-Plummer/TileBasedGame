// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TileType.generated.h"

// forward declaration of material instance
class UMaterialInstance;

USTRUCT(BlueprintType)
struct FTileType : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FTileType()
		: ID(0)
		, MoveCost(1)
		, DefenseModifier(0)
		, AttackModifier(0)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TileData)
	int32 ID; // the id of the tile in the data table of tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TileData)
	FName TypeName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TileData)
	int32 MoveCost; // cost of movement across the tile (if unit can traverse the tile)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TileData)
	int32 DefenseModifier; // additive defensive modifier. Affects damage recieved by a unit on this tile.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TileData)
	int32 AttackModifier; // additive attack modifier. Affects damage dealt by a unit on this tile.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TileData)
	UMaterialInstance* Material; // Pointer to material used on the tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TileData)
	UStaticMesh* Mesh; // Pointer to the mesh used by the tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TileData)
	FColor SourceImageColour; // color of this tile type in map source images (the 2d textures which are used as created map data)

};

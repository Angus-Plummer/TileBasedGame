// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TileData.generated.h"

// enum to list the different tile types
UENUM()
enum class ETileType : uint8
{
	BLANK		UMETA(DisplayName = "Blank Tile"),
	GRASS		UMETA(DisplayName = "Grass Tile"),
	FOREST		UMETA(DisplayName = "Forest Tile"),
	MOUNTAIN	UMETA(DisplayName = "Mountain Tile"),
	WATER		UMETA(DisplayName = "Water Tile")
};

// forward declaration of material instance
class UMaterialInstance;

USTRUCT(BlueprintType)
struct FTileData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FTileData()
		: TileType(ETileType::BLANK)
		, MoveCost(1)
		, DefenseModifier(0)
		, AttackModifier(0)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TileData)
	ETileType TileType; // name of the tile type
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

};

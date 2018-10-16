// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UENUM()
enum class ETileType: uint8 
{
	Blank		UMETA(DisplayName = "Blank Tile"),
	GRASS		UMETA(DisplayName = "Grass Tile"),
	FOREST		UMETA(DisplayName = "Forest Tile"),
	MOUNTAIN	UMETA(DisplayName = "Mountain Tile"),
	WATER		UMETA(DisplayName = "Water Tile")
};

// so TMap will work properly with ETileType keys
// uint8 GetTypeHash(const ETileType T)
// {
// 	return (uint8)T;
// }

/** A tile that can be clicked */
UCLASS()
class ATile : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Tile, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable tile */
	UPROPERTY(Category = Tile, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* TileMesh;

	UPROPERTY(EditAnywhere)
		ETileType TileType; // name of the tile type
	UPROPERTY(EditAnywhere)
		int32 MoveCost; // cost of movement across the tile (if unit can traverse the tile)
	UPROPERTY(EditAnywhere)
		int32 DefenseModifier; // additive defensive modifier. Affects damage recieved by a unit on this tile.
	UPROPERTY(EditAnywhere)
		int32 AttackModifier; // additive attack modifier. Affects damage dealt by a unit on this tile.
	UPROPERTY(EditAnywhere)
		class UMaterialInstance* Material; // Pointer to material used on the tile
	UPROPERTY(EditAnywhere)
		class UStaticMesh* Mesh; // Pointer to the mesh used by the tile

	

public:
	ATile();

	// updates the tile properties to those of the TileType 
	virtual void PostInitProperties() override;

	// if the TileType is changed then the other properties will be updated
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& change_event) override;

	/** Are we currently active? */
	bool bIsActive;

	/** Map that owns this tile */
	UPROPERTY()
	class ATileMap* OwningMap;

	// sets all the tile properties to those of the ATile in TileType
	UFUNCTION()
	void UpdateMesh();

	/** Handle the tile being clicked */
	UFUNCTION()
	void TileClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked);

	/** Handle the tile being touched  */
	UFUNCTION()
	void OnFingerPressedTile(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);
	
	UFUNCTION()
	void HandleClicked();
	
	UFUNCTION()
	void Highlight(bool bOn);

public:
	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns TileMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetTileMesh() const { return TileMesh; }
};




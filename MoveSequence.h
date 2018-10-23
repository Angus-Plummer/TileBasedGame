// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

// class for path finding (connects map coordinates together in sequences)
class MoveSequence
{
protected:
	FIntVector MapCoordinates; // the coordinates that this element in the move sequence corresponds to
	int32 MoveCost; // cost to traverse this coordinate
	MoveSequence* SequenceParent; // pointer to parent of this sequence element
	int CostToHere; // cost to reach this position from the starting point
	int HeuristicToTarget; // heuristic for cost to reach destination
public:
	// ctors and dtor
	MoveSequence();
	MoveSequence(FIntVector Coordinates, int32 Cost);
	MoveSequence(const MoveSequence& MoveSequence);
	~MoveSequence();

	// accessors and mutators
	void SetSequenceParent(MoveSequence& NewParent);
	const MoveSequence* GetSequenceParent() const;
	FIntVector GetMapCoordinates() const; // get coordinates that the move sequence object corresponds to
	const int& GetCostToHere() const; // returns CostToHere

	// set the heuristic for this tile to the target tile
	void Target(const FIntVector TargetTile);

	// get the score of the tile (cost to reach this coordinate + heuristic to reach target coordinate)
	const int Score() const;

	// updates the cost for this coordinate ( if parent has been changed etc)
	void Update();

	// copy constructor
	MoveSequence& operator=(const MoveSequence &rhs);

};

// type hash generator for move sequence elements just uses the map coordinates as these are unique
inline int32 GetTypeHash(const MoveSequence& MoveSequence)
{
	return GetTypeHash(MoveSequence.GetMapCoordinates());
}

// equality operator for move sequence elements
inline bool operator==(const MoveSequence& lhs, const MoveSequence& rhs)
{
	// only want to compare the map coordinates as really this is to check the underlying tile
	return lhs.GetMapCoordinates() == rhs.GetMapCoordinates();
}

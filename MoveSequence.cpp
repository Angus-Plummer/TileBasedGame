// Fill out your copyright notice in the Description page of Project Settings.

#include "MoveSequence.h"

// ---------- ctor, dtor, and operator overloading ---------- //

// ctors
MoveSequence::MoveSequence()
	: MapCoordinates(FIntVector(0,0,0))
	, MoveCost(0)
	, SequenceParent(nullptr)
	, CostToHere(0)
	, HeuristicToTarget(0)
{
}
MoveSequence::MoveSequence(FIntVector Coordinates, int32 Cost) 
	: MapCoordinates(Coordinates)
	, MoveCost(Cost)
	, SequenceParent(nullptr)
	, CostToHere(0)
	, HeuristicToTarget(0)
{
}
MoveSequence::MoveSequence(const MoveSequence& MoveSequence)
{
	MapCoordinates = MoveSequence.MapCoordinates;
	MoveCost = MoveSequence.MoveCost;
	SequenceParent = MoveSequence.SequenceParent;
	CostToHere = MoveSequence.CostToHere;
	HeuristicToTarget = MoveSequence.HeuristicToTarget;
}

// dtor
MoveSequence::~MoveSequence() {
}

// ---------- accessors and mutators ---------- //

// parent in chain
void MoveSequence::SetSequenceParent(MoveSequence &NewParent) {
	SequenceParent = &NewParent;
	Update();
}
const MoveSequence* MoveSequence::GetSequenceParent() const { return SequenceParent; }

// the coordinates that this element corresponds to
FIntVector MoveSequence::GetMapCoordinates() const { return MapCoordinates; }

// cost to reach this tile from the starting tile
const int& MoveSequence::GetCostToHere() const { return CostToHere; }

// ---------- other public functions ---------- //

// get the score of the tile (cost to reach tile + heuristic to reach target tile)
const int MoveSequence::Score() const {
	return CostToHere + HeuristicToTarget;
}

// get the heuristic for this element to the target coordinate. uses Manhattan distance
void MoveSequence::Target(FIntVector TargetCoordinates) {
	FIntVector Delta = TargetCoordinates - MapCoordinates;
	HeuristicToTarget = FMath::Abs(Delta.X) + FMath::Abs(Delta.Y);
}

// update the cost to reach this coordinate
void MoveSequence::Update() {
	// if tile has a parent then its cost is parents cost + the movement cost
	if (GetSequenceParent() != nullptr) {
		CostToHere = SequenceParent->CostToHere + MoveCost;
	}
	// if it has no parent then it is the starting coordinate and thus has 0 cost
	else {
		CostToHere = 0;
	}
}

MoveSequence& MoveSequence::MoveSequence::operator=(const MoveSequence &rhs)
{
	MapCoordinates = rhs.MapCoordinates;
	MoveCost = rhs.MoveCost;
	SequenceParent = rhs.SequenceParent;
	CostToHere = rhs.CostToHere;
	HeuristicToTarget = rhs.HeuristicToTarget;

	return *this;
}


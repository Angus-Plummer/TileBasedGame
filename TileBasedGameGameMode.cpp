// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TileBasedGameGameMode.h"
#include "TileBasedGamePlayerController.h"
#include "PlayerPawn.h"

ATileBasedGameGameMode::ATileBasedGameGameMode()
{
	// no pawn by default
	DefaultPawnClass = APlayerPawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = ATileBasedGamePlayerController::StaticClass();
}

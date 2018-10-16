// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TileBasedGamePlayerController.h"

ATileBasedGamePlayerController::ATileBasedGamePlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

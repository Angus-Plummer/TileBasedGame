// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PlayerPawn.h"
#include "TileMap.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// define game channels
#define ECC_Tile_Base_Meshes	ECC_GameTraceChannel1;

APlayerPawn::APlayerPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// only do tracing for tiles if there is a map connected
	if (Map)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
			{
				if (UCameraComponent* OurCamera = PC->GetViewTarget()->FindComponentByClass<UCameraComponent>())
				{
					FVector Start = OurCamera->GetComponentLocation();
					FVector End = Start + (OurCamera->GetComponentRotation().Vector() * 8000.0f);
					TraceForBlock(Start, End, true);
				}
			}
			else
			{
				FVector Start, Dir, End;
				PC->DeprojectMousePositionToWorld(Start, Dir);
				End = Start + (Dir * 8000.0f);
				TraceForBlock(Start, End, false);
			}
		}
	}
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("OnResetVR", EInputEvent::IE_Pressed, this, &APlayerPawn::OnResetVR);
	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &APlayerPawn::TriggerClick);
}

void APlayerPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, -90.0f, 0.0f);
}

void APlayerPawn::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void APlayerPawn::TriggerClick()
{
	if (Map->FocusedTileMesh->IsActive())
	{
		//Map->HandleClick();
	}
}

void APlayerPawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers)
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
	if (HitResult.Actor.IsValid())
	{
		FIntVector HitTilePosition = Map->WorldToMapCoordinates(HitResult.ImpactPoint);
		if (Map->Tiles.Contains(HitTilePosition))
		{
			Map->SelectFocusTile(HitTilePosition);
		}
		else
		{
			Map->UnsetFocusTile();
		}
	}
	else
	{
		Map->UnsetFocusTile();
	}
}
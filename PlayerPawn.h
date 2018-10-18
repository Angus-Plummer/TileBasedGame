// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS(config=Game)
class APlayerPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere)
	class ATileMap* Map;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

protected:
	void OnResetVR();
	void TriggerClick();
	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	//UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	//class ATile* CurrentTileFocus;
};

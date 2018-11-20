// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPSPLAYGROUND_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()



public:
	/** update camera when pawn dies */
	//virtual void PawnPendingDestroy(APawn* P) override;

	/** respawn after dying */
	virtual void UnFreeze() override;
	

};
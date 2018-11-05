// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSPlaygroundGameMode.h"
#include "PreMatchGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FPSPLAYGROUND_API APreMatchGameMode : public AFPSPlaygroundGameMode
{
	GENERATED_BODY()

public:
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;

private:
	uint32 NumberOfPlayers = 0;
};

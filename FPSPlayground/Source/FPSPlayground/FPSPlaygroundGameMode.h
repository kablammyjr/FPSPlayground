// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSPlaygroundGameMode.generated.h"

class AFPSPlaygroundCharacter;

UCLASS(minimalapi)
class AFPSPlaygroundGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPSPlaygroundGameMode();

	void SpawnPlayer(AController* Controller);

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void PlayerDeath(AController* Controller);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly)
	AFPSPlaygroundCharacter* FPSCharacter;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFPSPlaygroundCharacter> FPSCharacterBlueprint;

	UPROPERTY(EditDefaultsOnly)
	float RespawnDelay = 5.0f;

	void SpawnPlayerTimer(AController* Controller);
};




// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FPSPlaygroundGameMode.h"
#include "FPSPlaygroundHUD.h"
#include "FPSPlaygroundCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPSPlaygroundGameMode::AFPSPlaygroundGameMode(): Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Characters/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSPlaygroundHUD::StaticClass();
}

void AFPSPlaygroundGameMode::BeginPlay()
{
	Super::BeginPlay();

	FPSCharacter = Cast<AFPSPlaygroundCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AFPSPlaygroundGameMode::SpawnPlayer(AController* Controller)
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn from controller: %s"), *Controller->GetName());
	Controller->UnPossess();
}
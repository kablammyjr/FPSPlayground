// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FPSPlaygroundGameMode.h"
#include "FPSPlaygroundHUD.h"
#include "FPSPlaygroundCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "FPSPlaygroundCharacter.h"

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

void AFPSPlaygroundGameMode::PostLogin(APlayerController* NewPlayer)
{
	SpawnPlayer(NewPlayer);
}

void AFPSPlaygroundGameMode::SpawnPlayerTimer(AController* Controller)
{
	const float Interval = 3.f;
	const bool Loop = false;
	const float FirstDelay = 0.f;
	FTimerHandle UniqueHandle;
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &AFPSPlaygroundGameMode::SpawnPlayer, Controller);
	GetWorldTimerManager().SetTimer(UniqueHandle, RespawnDelegate, Interval, Loop, FirstDelay);
}

void AFPSPlaygroundGameMode::PlayerDeath(AController* Controller)
{
	UE_LOG(LogTemp, Warning, TEXT("Player died: %s"), *Controller->GetName());
	Controller->UnPossess();
	SpawnPlayerTimer(Controller);
}

void AFPSPlaygroundGameMode::SpawnPlayer(AController* Controller)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	FPSCharacter = GetWorld()->SpawnActor<AFPSPlaygroundCharacter>(FPSCharacterBlueprint, FVector(0.0f, 0.0f, 300.0f), FRotator(0.0f, 0.0f, 0.0f), ActorSpawnParams);
	Controller->Possess(FPSCharacter);
}
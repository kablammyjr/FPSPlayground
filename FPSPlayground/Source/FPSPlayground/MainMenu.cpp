// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"



bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(Host != nullptr)) return false;
	Host->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	return true;
}

void UMainMenu::HostServer()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/FirstPersonCPP/Maps/FirstPersonExampleMap"), TRAVEL_Absolute);
	UE_LOG(LogTemp, Warning, TEXT("Host"));
}
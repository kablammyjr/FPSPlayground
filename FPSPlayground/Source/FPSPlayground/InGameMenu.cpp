// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMenu.h"
#include "Components/Button.h"




bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(ResumeButton != nullptr)) return false;
	ResumeButton->OnClicked.AddDynamic(this, &UInGameMenu::ResumePressed);

	if (!ensure(DisconnectButton != nullptr)) return false;
	DisconnectButton->OnClicked.AddDynamic(this, &UInGameMenu::DisconnectPressed);

	return true;
}

void UInGameMenu::ResumePressed()
{
	Teardown();
}

void UInGameMenu::DisconnectPressed()
{
	if (MenuInterface != nullptr)
	{
		Teardown();
		MenuInterface->LoadMainMenuLevel();
	}
}
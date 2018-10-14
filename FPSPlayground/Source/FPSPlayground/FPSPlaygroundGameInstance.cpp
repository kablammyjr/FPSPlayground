// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlaygroundGameInstance.h"
#include "FPSPlayerController.h"
#include "Classes/Engine/Engine.h"
#include "OnlineSessionSettings.h"
#include "OnlineSessionInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.h"
#include "MenuWidget.h"

const static FName SESSION_NAME = TEXT("My Session Game");

UFPSPlaygroundGameInstance::UFPSPlaygroundGameInstance(const FObjectInitializer & ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/Menu/WBP_MainMenu"));
	if (!ensure(MenuBPClass.Class != nullptr)) return;

		MenuClass = MenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/Menu/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;

	InGameMenuClass = InGameMenuBPClass.Class;
}

void UFPSPlaygroundGameInstance::Init()
{

	UE_LOG(LogTemp, Warning, TEXT("Found class %s"), *MenuClass->GetName());
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	
	if (Subsystem != nullptr && Subsystem != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UFPSPlaygroundGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UFPSPlaygroundGameInstance::OnDestroySessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found no subsystem"));
	}
	
}

void UFPSPlaygroundGameInstance::LoadMainMenu()
{
	if (!ensure(MenuClass != nullptr)) return;

	Menu = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();

	Menu->SetMenuInterface(this);
}

void UFPSPlaygroundGameInstance::LoadInGameMenu()
{
	if (!ensure(MenuClass != nullptr)) return;

	UMenuWidget* Menu = CreateWidget<UMenuWidget>(this, InGameMenuClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();

	Menu->SetMenuInterface(this);
}

void UFPSPlaygroundGameInstance::Host()
{
	UE_LOG(LogTemp, Warning, TEXT("hostbbb"));
	if (Menu != nullptr)
	{ 
		Menu->Teardown();
	}

	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}
}

void UFPSPlaygroundGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
		return; 
	}
	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->ServerTravel("/Game/FirstPersonCPP/Maps/FirstPersonExampleMap?listen");
}

void UFPSPlaygroundGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		CreateSession();
	}
}

void UFPSPlaygroundGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

void UFPSPlaygroundGameInstance::Join(const FString& Address)
{
	if (Menu != nullptr)
	{
		Menu->Teardown();
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UFPSPlaygroundGameInstance::LoadMainMenuLevel()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel("/Game/Menu/MainMenu", ETravelType::TRAVEL_Absolute);
}
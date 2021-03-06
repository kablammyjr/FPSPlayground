// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 Currentplayers;
	uint16 MaxPlayers;
	FString HostUsername;
};

UCLASS()
class FPSPLAYGROUND_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OVERRIDES 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UMainMenu(const FObjectInitializer & ObjectInitializer);

protected:
	virtual bool Initialize();



/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COMPONENTS 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	

private:
	TSubclassOf<class UUserWidget> ServerListClass;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenuQuitButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinMenuJoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinMenuBackButton;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenuBackButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenuHostButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* HostChosenServerName;

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;

TOptional<uint32> SelectedIndex;



/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COMPONENT FUNCTIONS 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	void SelectIndex(uint32 Index);
	void SetServerList(TArray<FServerData> ServerNames);

protected:
	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void OpenJoinMenu();

	void UpdateChildren();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void QuitGame();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"
#include "FPSPlaygroundGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPSPLAYGROUND_API UFPSPlaygroundGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OVERRIDES 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UFPSPlaygroundGameInstance(const FObjectInitializer & ObjectInitializer);

	virtual void Init();
	virtual void LoadMainMenuLevel() override;
	virtual void RefreshServerList();




/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MENUS 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable)
	void LoadInGameMenu();

private:
	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;

	class UMainMenu* Menu;





/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SESSIONS 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UFUNCTION()
	void Host(FString ServerName) override;

	UFUNCTION(Exec)
	void Join(uint32 Index) override;

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void CreateSession();
	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool Success);

	FString DesiredServerName;
};

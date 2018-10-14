// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuInterface.h"
#include "OnlineSubsystem.h"
#include "FPSPlaygroundGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPSPLAYGROUND_API UFPSPlaygroundGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:
	UFPSPlaygroundGameInstance(const FObjectInitializer & ObjectInitializer);
	
	virtual void Init();

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	UFUNCTION(Exec)
	void Host();

	UFUNCTION(Exec)
	void Join(const FString& Address);

private:

	TSubclassOf<class UUserWidget> MenuClass;

	IOnlineSessionPtr SessionInterface;

	void OnCreateSessionComplete(FName SessionName, bool Success);

	void OnDestroySessionComplete(FName SessionName, bool Success);

	void CreateSession();
};

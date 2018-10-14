// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.h"
#include "InGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class FPSPLAYGROUND_API UInGameMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize();
	
private:
	UPROPERTY(meta = (BindWidget))
	class UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* DisconnectButton;
	
	UFUNCTION()
	void ResumePressed();

	UFUNCTION()
	void DisconnectPressed();
};

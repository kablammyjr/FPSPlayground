// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerList.generated.h"

/**
 * 
 */
UCLASS()
class FPSPLAYGROUND_API UServerList : public UUserWidget
{
	GENERATED_BODY()
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COMPONENTS 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostUsername;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerCount;

	UPROPERTY(BlueprintReadOnly)
	bool Selected = false;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* ListButton;

	UPROPERTY()
	class UMainMenu* Parent;

	uint32 Index;

/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COMPONENT FUNCTIONS 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	void Setup(class UMainMenu* InParent, uint32 InIndex);

private:
	UFUNCTION()
	void OnClicked();
	
	
};

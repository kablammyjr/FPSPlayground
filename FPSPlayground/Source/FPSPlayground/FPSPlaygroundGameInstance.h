// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FPSPlaygroundGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPSPLAYGROUND_API UFPSPlaygroundGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFPSPlaygroundGameInstance(const FObjectInitializer & ObjectInitializer);
	
	virtual void Init();

	UFUNCTION(Exec)
	void Host();

	UFUNCTION(Exec)
	void Join(const FString& Address);
};

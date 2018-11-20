// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FPSPLAYGROUND_API AFPSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
//	/** player killed someone */
//	void ScoreKill(AFPSPlayerState* Victim, int32 Points);
//
//	/** player died */
//	void ScoreDeath(AFPSPlayerState* KilledBy, int32 Points);
//
//	/** Sends kill (excluding self) to clients */
//	UFUNCTION(Reliable, Client)
//	void InformAboutKill(class AFPSPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AFPSPlayerState* KilledPlayerState);
//	void InformAboutKill_Implementation(class AFPSPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AFPSPlayerState* KilledPlayerState);
//
//	/** broadcast death to local clients */
//	UFUNCTION(Reliable, NetMulticast)
//	void BroadcastDeath(class AFPSPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AFPSPlayerState* KilledPlayerState);
//	void BroadcastDeath_Implementation(class AFPSPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AFPSPlayerState* KilledPlayerState);
//	
//protected:
//
//	/** team number */
//	UPROPERTY(Transient, ReplicatedUsing = OnRep_TeamColor)
//	int32 TeamNumber;
//
//	/** number of kills */
//	UPROPERTY(Transient, Replicated)
//	int32 NumKills;
//
//	/** number of deaths */
//	UPROPERTY(Transient, Replicated)
//	int32 NumDeaths;
//
//	/** helper for scoring points */
//	void ScorePoints(int32 Points);
	
};

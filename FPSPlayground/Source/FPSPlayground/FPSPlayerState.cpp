// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerState.h"
#include "FPSPlayground.h"
#include "FPSPlayerController.h"


// void AFPSPlayerState::ScoreKill(AFPSPlayerState* Victim, int32 Points)
//{
//	NumKills++;
//	ScorePoints(Points);
//}
//
//void AFPSPlayerState::ScoreDeath(AFPSPlayerState* KilledBy, int32 Points)
//{
//	NumDeaths++;
//	ScorePoints(Points);
//}
//
//void AFPSPlayerState::InformAboutKill_Implementation(class AFPSPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AFPSPlayerState* KilledPlayerState)
//{
//	//id can be null for bots
//	if (KillerPlayerState->UniqueId.IsValid())
//	{
//		//search for the actual killer before calling OnKill()	
//		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
//		{
//			AFPSPlayerController* TestPC = Cast<AFPSPlayerController>(*It);
//			if (TestPC && TestPC->IsLocalController())
//			{
//				// a local player might not have an ID if it was created with CreateDebugPlayer.
//				ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(TestPC->Player);
//				FUniqueNetIdRepl LocalID = LocalPlayer->GetCachedUniqueNetId();
//				if (LocalID.IsValid() && *LocalPlayer->GetCachedUniqueNetId() == *KillerPlayerState->UniqueId)
//				{
//					TestPC->OnKill();
//				}
//			}
//		}
//	}
//}
//
//void AFPSPlayerState::BroadcastDeath_Implementation(class AFPSPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AFPSPlayerState* KilledPlayerState)
//{
//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
//	{
//		// all local players get death messages so they can update their huds.
//		AFPSPlayerController* TestPC = Cast<AFPSPlayerController>(*It);
//		if (TestPC && TestPC->IsLocalController())
//		{
//			TestPC->OnDeathMessage(KillerPlayerState, this, KillerDamageType);
//		}
//	}
//}
//
//void AFPSPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(AFPSPlayerState, TeamNumber);
//	DOREPLIFETIME(AFPSPlayerState, NumKills);
//	DOREPLIFETIME(AFPSPlayerState, NumDeaths);
//}


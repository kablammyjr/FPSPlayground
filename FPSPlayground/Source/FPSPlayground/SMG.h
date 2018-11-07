// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMG.generated.h"

class AFPSPlaygroundCharacter;

UCLASS()
class FPSPLAYGROUND_API ASMG : public AActor
{
	GENERATED_BODY()
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OVERRIDES 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	// Sets default values for this actor's properties
	ASMG();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COMPONENTS 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* SMGFireAnimation1P;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	class USoundBase* SMGFireSound;

private:
	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* Root;

	/*UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* SMGMesh;*/

	AFPSPlaygroundCharacter* FPSCharacter;
	




/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FIRING 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UFUNCTION()
	void OnFireSMG();
	UFUNCTION()
	void OnContinuousFireSMG();
	UFUNCTION()
	void StopFireSMG();
	UFUNCTION()
	void PlayRecoilAndSoundSMG();

private:
	void CanContinueFiring();

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float FireRate = 0.08f;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float ShootDelay = 0.08f;

	void CanShoot();
	void CanRecoil();

	bool bCanShoot = true;
	bool bIsFiring = false;
	bool bCanRecoil = true;

	bool bIsCrouching = false;
	bool bIsADS = false;
	bool bIsMoving = false;

	FRotator BulletRotation;




/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GETTERS 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UFUNCTION(BlueprintCallable)
	bool GetIsFiring() { return bIsFiring; }

	UFUNCTION(BlueprintCallable)
	bool GetCanFireGun() { return bCanShoot; }
};

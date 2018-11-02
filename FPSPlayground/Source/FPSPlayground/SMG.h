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

	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* SMGMesh;

	APlayerController* PlayerController;

	AFPSPlaygroundCharacter* FPSCharacter;
	
public:	
	// Sets default values for this actor's properties
	ASMG();

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* SMGFireAnimation1P;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	class USoundBase* SMGFireSound;



	UFUNCTION()
	void OnFireSMG();
	UFUNCTION()
	void OnContinuousFireSMG();
	UFUNCTION()
	void StopFireSMG();



	UFUNCTION(BlueprintCallable)
	bool GetIsFiring();

	UFUNCTION(BlueprintCallable)
	bool GetCanFireGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
	void PlayRecoilAndSoundSMG();

	void CanShoot();
	void CanContinueFiring();
	void CanRecoil();



	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float FireRate = 0.08f;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float ShootDelay = 0.08f;


	bool bCanShoot = true;

	bool bIsFiring = false;

	bool bCanRecoil = true;

	bool bIsCrouching = false;
	
	bool bIsADS = false;

	bool bIsMoving = false;

	FRotator BulletRotation;
	
};

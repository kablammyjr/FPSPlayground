// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSPlaygroundProjectile.h"
#include "SMG.generated.h"

class AFPSPlaygroundCharacter;

UCLASS()
class FPSPLAYGROUND_API ASMG : public AActor
{
	GENERATED_BODY()

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;
	
public:	
	// Sets default values for this actor's properties
	ASMG();

	UFUNCTION(BlueprintCallable)
	void IsMoving(bool IsMoving);

	UFUNCTION(BlueprintCallable)
	void IsADS(bool ADSStatus);

	UFUNCTION(BlueprintCallable)
	void IsCrouched(bool CrouchStatus);

	void OnRelease();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OnFire();

	UFUNCTION(BlueprintCallable)
	void OnContinuousFire();

	void CanShoot();

	UFUNCTION(Category = "Animation", BlueprintImplementableEvent)
	void RecoilEvent();

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFPSPlaygroundProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	UFUNCTION(BlueprintCallable)
	void MuzzleWorldLocationRotation(FVector MuzzleWorldLocation, FRotator MuzzleWorldRotation);


private:

	UPROPERTY(EditAnywhere, Category = "Firing")
	float FireRate = 0.08;

	UPROPERTY(EditAnywhere, Category = "Firing")
	float ShootDelay = 0.01;

	bool bCanShoot = true;

	bool bIsFiring = false;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	class AFPSPlaygroundCharacter* FPSCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<class ASMG> SMGBlueprint;

	FRotator BulletRotation;

	bool bIsMoving;

	bool bIsADS;

	bool bIsCrouched;

	FVector FMuzzleWorldLocation;

	FRotator FMuzzleWorldRotation;
};

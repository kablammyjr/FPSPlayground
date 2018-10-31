// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Character.h"
#include "FPSPlaygroundCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AFPSPlaygroundCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* GunMesh1P;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* GunMesh3P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFPSPlaygroundProjectile> ProjectileClass;

	class UCharacterMovementComponent* CharacterMovementComponent;

public:
	AFPSPlaygroundCharacter();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh3P;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UFUNCTION(BlueprintCallable)
	bool GetIsFiring();

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* MuzzleLocation;

	UFUNCTION()
	void PlayRecoilAnimationAndSoundSMG();

	UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable)
	void Server_PlayRecoilAnimationAndSoundSMG(AActor* Actor, USoundBase* Sound, FVector Location);
	void Server_PlayRecoilAnimationAndSoundSMG_Implementation(AActor* Actor, USoundBase* Sound, FVector Location);
	bool Server_PlayRecoilAnimationAndSoundSMG_Validate(AActor* Actor, USoundBase* Sound, FVector Location);

	void OnADS();

	UFUNCTION(BlueprintCallable)
	void ReleaseADS();

	UFUNCTION(BlueprintCallable)
	bool GetCanFireGun();

	bool GetIsCrouched();

	bool bIsSprinting = false;

	UFUNCTION(BlueprintCallable)
	bool GetIsADS();

	UFUNCTION(Category = "Movement", BlueprintImplementableEvent)
	void SetOnSprint();

	UFUNCTION(Category = "Movement", BlueprintImplementableEvent)
	void SetStopSprint();

	UPROPERTY()
	bool bIsADS = false;

	UFUNCTION()
	void OnFireSMG();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnFireSMG(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);
	void Server_OnFireSMG_Implementation(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);
	bool Server_OnFireSMG_Validate(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);

	UFUNCTION()
	void OnContinuousFireSMG();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnContinuousFireSMG(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);
	void Server_OnContinuousFireSMG_Implementation(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);
	bool Server_OnContinuousFireSMG_Validate(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);

	UFUNCTION()
	void StopFireSMG();

protected:

	UFUNCTION()
	void PullTrigger();

	UFUNCTION()
	void ReleaseTrigger();

	void StartCrouch();
	void StopCrouch();

	void OnSprint();
	void StopSprint();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:

	UFUNCTION(BlueprintCallable)
	void GetIsMoving(bool IsMoving);

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	class USoundBase* SMGFireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float FireRate = 0.08f;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float ShootDelay = 0.08f;

	void CanShoot();
	void CanContinueFiring();

	bool bCanShoot = true;

	void CanRecoil();

	bool bIsFiring = false;

	bool bCanRecoil = true;

	bool bIsWalkingForward = false;

	float MoveForwardAxis;

	bool bOnSprint = false;

	bool bIsMoving;

	UAnimInstance* Mesh1PAnimInstance;

	FRotator BulletRotation;
	FRotator SpawnRotation;
	FVector SpawnLocation;
};


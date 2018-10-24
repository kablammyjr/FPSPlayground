// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Character.h"
#include "FPSPlaygroundCharacter.generated.h"

class UInputComponent;
class ASMG;

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
	class USkeletalMeshComponent* Mesh3P;

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

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<class ASMG> SMGBlueprint;

	UFUNCTION(BlueprintCallable)
	bool GetIsFiring();

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	UPROPERTY(BlueprintReadOnly)
	class USceneComponent* MuzzleLocation;

	UFUNCTION(BlueprintCallable)
	void PlayRecoilAnimation();

	UFUNCTION(BlueprintCallable)
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

	bool bIsADS = false;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnFireSMG(FRotator CameraRotation);
	void Server_OnFireSMG_Implementation(FRotator CameraRotation);
	bool Server_OnFireSMG_Validate(FRotator CameraRotation);

	UFUNCTION()
	void OnContinuousFireSMG();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnContinuousFireSMG(FRotator CameraRotation);
	void Server_OnContinuousFireSMG_Implementation(FRotator CameraRotation);
	bool Server_OnContinuousFireSMG_Validate(FRotator CameraRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopFireSMG();
	void Server_StopFireSMG_Implementation();
	bool Server_StopFireSMG_Validate();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Setup, meta = (AllowPrivateAccess = "true"))
	ASMG* SMG;

	UFUNCTION(BlueprintCallable)
	void GetIsMoving(bool IsMoving);

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, Category = Gameplay)
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = "Firing")
	float FireRate = 0.08;

	void CanShoot();

	bool bCanShoot = true;

	void CanRecoil();

	bool bIsFiring = false;

	bool bCanRecoil = true;

	bool bIsWalkingForward = false;

	float MoveForwardAxis;

	bool bOnSprint = false;

	bool bIsMoving;

	FRotator BulletRotation;
	FRotator SpawnRotation;
	FVector SpawnLocation;
};


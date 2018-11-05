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

/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OVERRIDES 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	AFPSPlaygroundCharacter();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay();





/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COMPONENTS 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh3P;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Weapons)
	ASMG* SMG;

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

private:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditDefaultsOnly, Category = Weapons)
	TSubclassOf<ASMG> SMGBlueprint;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* GunMesh3P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFPSPlaygroundProjectile> ProjectileClass;

	class UCharacterMovementComponent* CharacterMovementComponent;

	UAnimInstance* Mesh1PAnimInstance;
	UAnimInstance* Mesh3PAnimInstance;




	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MOVEMENT 
	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


public:
	// Base turn rate, in deg/sec. Other scaling may affect final turn rate.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	// Base look up/down rate, in deg/sec. Other scaling may affect final rate.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:
	// Handles moving forward/backward
	void MoveForward(float Val);

	// Handles stafing movement, left and right
	void MoveRight(float Val);

	// Called via input to turn at a given rate.
	// @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate 
	void TurnAtRate(float Rate);

	// Called via input to turn look up/down at a given rate.
	// @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	void LookUpAtRate(float Rate);

	void StartCrouch();
	void StopCrouch();

private:
	float MoveForwardAxis;





	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// FIRING 
	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	public:
	UFUNCTION()
	void OnFire(FRotator BulletRotation);

	UFUNCTION()
	void FireAnimationSMG(UAnimMontage* FireAnimation1P);

	UFUNCTION()
	void FireSoundSMG(USoundBase* Sound);

	UFUNCTION()
	void OnADS();

	UFUNCTION(BlueprintCallable)
	void ReleaseADS();

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* SMGFireAnimation3P;

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnedWeapon(); 

	UPROPERTY()
	bool bIsADS = false;

	///////////// RPCs START ///////////////

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_FireSoundSMG(USoundBase* Sound);
	void Server_FireSoundSMG_Implementation(USoundBase* Sound);
	bool Server_FireSoundSMG_Validate(USoundBase* Sound);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnFireSMG(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);
	void Server_OnFireSMG_Implementation(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);
	bool Server_OnFireSMG_Validate(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);

	///////////// RPCs END ///////////////

protected:
	UFUNCTION()
	void PullTrigger();

	UFUNCTION()
	void ReleaseTrigger();





	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GETTERS 
	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	UFUNCTION(BlueprintCallable)
	bool GetIsMoving();

	bool GetIsCrouched() { return bIsCrouched; }

	UFUNCTION(BlueprintCallable)
	bool GetIsADS() { return bIsADS; }

	// Returns Mesh1P subobject
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	// Returns FirstPersonCameraComponent subobject
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};
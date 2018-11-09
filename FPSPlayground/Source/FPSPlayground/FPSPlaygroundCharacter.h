// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Character.h"
#include "FPSPlaygroundCharacter.generated.h"

class UInputComponent;
class ASMG;
class AFPSPlaygroundProjectile;

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
	// Sets up player input
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;




	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// COMPONENTS 
	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* BulletCollision;

	// Location in front of the character that projectiles spawn at
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* MuzzleLocation;

	// Character mesh that only other players can see
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh3P;

protected:

	// SMG class object
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Weapons)
	ASMG* SMG;

	// SMG blueprint class object
	UPROPERTY(EditDefaultsOnly, Category = Weapons)
	TSubclassOf<ASMG> SMGBlueprint;

	// Bullet class object
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Weapons)
	AFPSPlaygroundProjectile* BulletClass;

private:
	// Character arms mesh that is only seen by the owning player
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	// Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<AFPSPlaygroundProjectile> BulletBlueprint;

	// Gun mesh to be attached to the arms of the character mesh seen only by owning player
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* GunMesh1P;

	// Gun mesh to be attached to the arms of the character mesh seen by only other players
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* GunMesh3P;

	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	// Sets up character movement
	class UCharacterMovementComponent* CharacterMovementComponent;
	
	// To store the animinstance set for both character meshes
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

	// Called via input to make your character crouch
	void StartCrouch();
	void StopCrouch();

private:
	// The amount of throttle given to forward movement
	float MoveForwardAxis;





	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// FIRING 
	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	// Called via input | Calculates bullet rotation and calls the server to spawn the projectile
	UFUNCTION()
	void OnFire(FRotator BulletRotation);
	// Plays the animation for gun recoil
	UFUNCTION()
	void FireAnimationSMG(UAnimMontage* FireAnimation1P);
	// Plays a sound when the weapon is fired
	UFUNCTION()
	void FireSoundSMG(USoundBase* Sound);
	// Called via input | Zooms camera in and increases gun accuracy
	UFUNCTION()
	void OnADS();
	UFUNCTION()
	void ReleaseADS();
	//AnimMontage to play each time we fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* SMGFireAnimation3P;
	// Event that triggers after the weapon has spawned
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnedWeapon(); 

	///////////// RPCs START ///////////////

	// Plays gun firing sound at a location on the server
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_FireSoundSMG(USoundBase* Sound);
	void Server_FireSoundSMG_Implementation(USoundBase* Sound);
	bool Server_FireSoundSMG_Validate(USoundBase* Sound);

	// Spawns a projectile on the server
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnFireSMG(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);
	void Server_OnFireSMG_Implementation(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);
	bool Server_OnFireSMG_Validate(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn);

	///////////// RPCs END ///////////////

protected:
	// Called via input | Fires current equipped weapon
	UFUNCTION()
	void PullTrigger();
	UFUNCTION()
	void ReleaseTrigger();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateADSFOV();

private:
	// Returns true or false when the ADS input is pressed or released
	UPROPERTY()
	bool bIsADS = false;






	/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
	// ON TAKE DAMAGE
	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;
				
		

protected:
	UPROPERTY(EditAnywhere, Category = Damage)
	float Health = 100;

	UFUNCTION()
	void Dead();

	UFUNCTION()
	void DestroyAfterDeath();

private:
	class AFPSPlaygroundGameMode* GameMode;







	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GETTERS 
	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	// Returns whether the character is currently moving
	UFUNCTION(BlueprintCallable)
	bool GetIsMoving();
	// Returns whether the character is currently crouching
	bool GetIsCrouched() { return bIsCrouched; }
	// Returns whether the character is currently aiming down sights
	UFUNCTION(BlueprintCallable)
	bool GetIsADS() { return bIsADS; }

	// Returns Mesh1P subobject
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	// Returns FirstPersonCameraComponent subobject
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};
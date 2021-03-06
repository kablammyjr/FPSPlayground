// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSPlaygroundProjectile.generated.h"

class AFPSPlaygroundCharacter;

UCLASS(config=Game)
class AFPSPlaygroundProjectile : public AActor
{
	GENERATED_BODY()
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OVERRIDES 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	AFPSPlaygroundProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COMPONENTS 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* MuzzleFlash;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVEMENT 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	void MakeVisible();

	UPROPERTY(EditAnywhere, Category = "Setup")
	float TimeUntilVisible = 0.005f;



/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ON HIT 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:

	AFPSPlaygroundCharacter* FPSCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* BulletHit;

	FTransform BulletHitTransform;

	UPROPERTY(EditDefaultsOnly)
	int32 BulletDamage = 10;



	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GETTERS 
	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	int32 GetBulletDamage() { return BulletDamage; }
};


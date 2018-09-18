// Fill out your copyright notice in the Description page of Project Settings.

#include "SMG.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "FPSPlaygroundCharacter.h"


// Sets default values
ASMG::ASMG()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	//FP_Gun->SetupAttachment(RootComponent);
	RootComponent = FP_Gun;

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	//FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(0.0f, 0.0f, 0.0f); 

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
}

// Called when the game starts or when spawned
void ASMG::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASMG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASMG::OnFire()
{
	if (bCanShoot)
	{
		bIsFiring = true;
		// try and fire a projectile
		if (ProjectileClass != NULL)
		{
			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				if (bIsFiring)
				{
					if (bIsMoving)
					{
						BulletRotation = FRotator(FMath::RandRange(-4.0f, 4.0f), FMath::RandRange(-4.0f, 4.0f), 0.0f);
					} 
					else
					{
						BulletRotation = FRotator(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 0.0f);
					}

					const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation() + BulletRotation;
					// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation())/** + SpawnRotation.RotateVector(GunOffset)*/;

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

					// spawn the projectile at the muzzle
					World->SpawnActor<AFPSPlaygroundProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

					FTimerHandle FuzeTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ASMG::OnContinuousFire, FireRate, false);
				}
			}
		}
		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
}

void ASMG::OnContinuousFire()
{
	if (bIsFiring)
	{
		bIsFiring = true;
		// try and fire a projectile
		if (ProjectileClass != NULL)
		{
			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				if (bIsFiring)
				{			
					BulletRotation = FRotator(FMath::RandRange(-4.0f, 4.0f), FMath::RandRange(-4.0f, 4.0f), 0.0f);
					
					const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation() + BulletRotation;
					// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation())/** + SpawnRotation.RotateVector(GunOffset)*/;

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

					// spawn the projectile at the muzzle
					World->SpawnActor<AFPSPlaygroundProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

					if (bIsFiring)
					{
						FTimerHandle FuzeTimerHandle;
						GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ASMG::OnContinuousFire, FireRate, false);
					}
					else { return; }
				}
			}
		}
		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
}

void ASMG::OnRelease()
{
	bCanShoot = false;
	bIsFiring = false;
	//FTimerHandle FuzeTimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ASMG::CanShoot, ShootDelay, false);
	CanShoot();
}

void ASMG::CanShoot()
{
	bCanShoot = true;
}

void ASMG::IsMoving(bool IsMoving)
{
	bIsMoving = IsMoving;
}


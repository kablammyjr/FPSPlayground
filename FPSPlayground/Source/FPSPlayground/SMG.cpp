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

	RootComponent = FP_Gun;

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);

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
					if (bIsCrouched)
					{
						if (bIsADS)
						{
							if (bIsMoving)
							{
								BulletRotation = FRotator(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 0.0f);
							}
							else
							{
								BulletRotation = FRotator(FMath::RandRange(-0.5f, 0.5f), FMath::RandRange(-0.5f, 0.5f), 0.0f);
							}
						}
						else
						{
							if (bIsMoving)
							{
								BulletRotation = FRotator(FMath::RandRange(-2.5f, 2.5f), FMath::RandRange(-2.5f, 2.5f), 0.0f);
							}
							else
							{
								BulletRotation = FRotator(FMath::RandRange(-1.5f, 1.5f), FMath::RandRange(-1.5f, 1.5f), 0.0f);
							}
						}
					}
					else if (bIsADS)
					{
						if (bIsMoving)
						{
							BulletRotation = FRotator(FMath::RandRange(-4.0f, 4.0f), FMath::RandRange(-4.0f, 4.0f), 0.0f);
						}
						else
						{
							BulletRotation = FRotator(FMath::RandRange(-3.0f, 3.0f), FMath::RandRange(-3.0f, 3.0f), 0.0f);
						}
					}
					else
					{
						if (bIsMoving)
						{
							BulletRotation = FRotator(FMath::RandRange(-8.0f, 8.0f), FMath::RandRange(-8.0f, 8.0f), 0.0f);
						}
						else
						{
							BulletRotation = FRotator(FMath::RandRange(-6.0f, 6.0f), FMath::RandRange(-6.0f, 6.0f), 0.0f);
						}
					}
					
					//const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation() + BulletRotation;
					// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					//const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation());

					RecoilEvent();

					const FRotator SpawnRotation = FMuzzleWorldRotation + BulletRotation;

					const FVector SpawnLocation = FMuzzleWorldLocation;

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
					if (bIsCrouched)
					{
						if (bIsADS)
						{
							if (bIsMoving)
							{
								BulletRotation = FRotator(FMath::RandRange(-2.0f, 2.0f), FMath::RandRange(-2.0f, 2.0f), 0.0f);
							}
							else
							{
								BulletRotation = FRotator(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 0.0f);
							}
						}
						else
						{
							if (bIsMoving)
							{
								BulletRotation = FRotator(FMath::RandRange(-3.5f, 3.5f), FMath::RandRange(-3.5f, 3.5f), 0.0f);
							}
							else
							{
								BulletRotation = FRotator(FMath::RandRange(-2.5f, 2.5f), FMath::RandRange(-2.5f, 2.5f), 0.0f);
							}
						}
					}
					else if (bIsADS)
					{
						if (bIsMoving)
						{
							BulletRotation = FRotator(FMath::RandRange(-5.0f, 5.0f), FMath::RandRange(-5.0f, 5.0f), 0.0f);
						}
						else
						{
							BulletRotation = FRotator(FMath::RandRange(-4.0f, 4.0f), FMath::RandRange(-4.0f, 4.0f), 0.0f);
						}
					}
					else
					{
						BulletRotation = FRotator(FMath::RandRange(-8.0f, 8.0f), FMath::RandRange(-8.0f, 8.0f), 0.0f);
					}
					
					//const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation() + BulletRotation;
					// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					//const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation());

					const FRotator SpawnRotation = FMuzzleWorldRotation + BulletRotation;

					const FVector SpawnLocation = FMuzzleWorldLocation;

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

					RecoilEvent();

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

void ASMG::IsADS(bool ADSStatus)
{
	bIsADS = ADSStatus;
}

void ASMG::IsCrouched(bool CrouchStatus)
{
	bIsCrouched = CrouchStatus;
}

void ASMG::MuzzleWorldLocationRotation(FVector MuzzleWorldLocation, FRotator MuzzleWorldRotation)
{
	FMuzzleWorldLocation = MuzzleWorldLocation;
	FMuzzleWorldRotation = MuzzleWorldRotation;
}


// Fill out your copyright notice in the Description page of Project Settings.

#include "SMG.h"
#include "FPSPlaygroundCharacter.h"
#include "Kismet/GameplayStatics.h"

/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OVERRIDES 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets default values
ASMG::ASMG()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//SMGMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SMGMesh"));

	RootComponent = Root;
}

// Called when the game starts or when spawned
void ASMG::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Begin play: %s"),*this->GetName());

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstLocalPlayerFromController()->GetPlayerController(World);
	if (!ensure(PlayerController != nullptr)) return;

	/*this->SetOwner(PlayerController);*/

	FPSCharacter = Cast<AFPSPlaygroundCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
}

// Called every frame
void ASMG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}








/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FIRING 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ASMG::OnFireSMG()
{
	bIsCrouching = FPSCharacter->GetIsCrouched();
	bIsADS = FPSCharacter->GetIsADS();
	bIsMoving = FPSCharacter->GetIsMoving();

	bIsFiring = true;
	if (bCanShoot)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bIsFiring)
			{
				if (bIsCrouching)
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

				FPSCharacter->OnFire(BulletRotation);
				//PlayRecoilAndSoundSMG();

				FTimerHandle FuzeTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ASMG::CanContinueFiring, FireRate, false);
			}
		}
	}
}

void ASMG::OnContinuousFireSMG()
{
	bIsCrouching = FPSCharacter->GetIsCrouched();
	bIsADS = FPSCharacter->GetIsADS();
	bIsMoving = FPSCharacter->GetIsMoving();

	if (bCanShoot)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bIsFiring)
			{
				if (bIsCrouching)
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

				FPSCharacter->OnFire(BulletRotation);
				//PlayRecoilAndSoundSMG();

				FTimerHandle FuzeTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ASMG::CanContinueFiring, FireRate, false);
			}
		}
	}
}

void ASMG::StopFireSMG()
{
	bIsFiring = false;
	bCanShoot = false;

	FTimerHandle FuzeTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ASMG::CanShoot, ShootDelay, false);
}

void ASMG::PlayRecoilAndSoundSMG()
{
	if (SMGFireSound != nullptr && SMGFireAnimation1P != nullptr)
	{
		if (bCanRecoil)
		{
			FPSCharacter->FireSoundSMG(SMGFireSound);
			FPSCharacter->FireAnimationSMG(SMGFireAnimation1P);

			bCanRecoil = false;
			FTimerHandle FuzeTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ASMG::CanRecoil, 0.1f, false);
		}
	}
}

void ASMG::CanContinueFiring()
{
	if (bCanShoot)
	{
		if (bIsFiring)
		{
			OnContinuousFireSMG();
		}
	}
}

void ASMG::CanShoot()
{
	bCanShoot = true;
}

void ASMG::CanRecoil()
{
	bCanRecoil = true;
}
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FPSPlaygroundCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "UnrealNetwork.h"
#include "FPSPlaygroundProjectile.h"
#include "FPSPlayerController.h"
#include "FPSPlaygroundGameMode.h"
#include "SMG.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);



/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OVERRIDES 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AFPSPlaygroundCharacter::AFPSPlaygroundCharacter()
{
	bReplicates = true;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	/////////////// COMPONENTS //////////////////

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	RootComponent = GetCapsuleComponent();

	BulletCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BulletCollision"));
	BulletCollision->SetupAttachment(GetCapsuleComponent());

	// Create and setup CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a character mesh component that will only be seen by owner
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a character mesh component that will only be seen by others
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->bOwnerNoSee = true;
	Mesh3P->bCastDynamicShadow = false;
	Mesh3P->CastShadow = false;
	Mesh3P->SetupAttachment(GetCapsuleComponent());

	// Create a gun mesh that will only be seen by others
	GunMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh1P"));
	GunMesh1P->bOnlyOwnerSee = true;
	GunMesh1P->bCastDynamicShadow = false;
	GunMesh1P->CastShadow = false;
	GunMesh1P->SetupAttachment(Mesh1P);

	// Create a gun mesh that will only be seen by others
	GunMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh3P"));
	GunMesh3P->bOwnerNoSee = true; 
	GunMesh3P->bCastDynamicShadow = false;
	GunMesh3P->CastShadow = false;
	GunMesh3P->SetupAttachment(Mesh3P);

	// Create a location in front of camera to spawn projectiles at
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(FirstPersonCameraComponent);
}

void AFPSPlaygroundCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//GameMode = Cast<AFPSPlaygroundGameMode>(GetWorld()->GetAuthGameMode());

	// SMG spawn and setup
	if (SMGBlueprint == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("SMG blueprint missing."));
		return;
	}
	
	SMG = GetWorld()->SpawnActor<ASMG>(SMGBlueprint);

	UE_LOG(LogTemp, Warning, TEXT("Spawning: %s"), *SMG->GetName());
	SMG->SetOwner(this);
	
	SMG->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
	// Attach gun mesh to 3rd person mesh to be seen only by self
	GunMesh1P->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	// Attach gun mesh to 3rd person mesh to be seen by others
	GunMesh3P->AttachToComponent(Mesh3P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Stores 1st and 3rd person mesh's animinstances
	Mesh1PAnimInstance = Mesh1P->GetAnimInstance();
	Mesh3PAnimInstance = Mesh3P->GetAnimInstance();

	// Event to declare that the weapon has been spawned
	SpawnedWeapon();
}

void AFPSPlaygroundCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFPSPlaygroundCharacter::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	// Only replicate this property for a short duration after it changes so join in progress players don't get spammed with fx when joining late
	DOREPLIFETIME_ACTIVE_OVERRIDE(AFPSPlaygroundCharacter, LastTakeHitInfo, GetWorld() && GetWorld()->GetTimeSeconds() < LastTakeHitTimeTimeout);
}

void AFPSPlaygroundCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AFPSPlaygroundCharacter, LastTakeHitInfo, COND_Custom);

	// everyone
	DOREPLIFETIME(AFPSPlaygroundCharacter, Health);
}





/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INPUT
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AFPSPlaygroundCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind crouch events
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSPlaygroundCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFPSPlaygroundCharacter::StopCrouch);

	// Bind fire events
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSPlaygroundCharacter::PullTrigger);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPSPlaygroundCharacter::ReleaseTrigger);

	// Bind aim down sights events
	PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &AFPSPlaygroundCharacter::OnADS);
	PlayerInputComponent->BindAction("ADS", IE_Released, this, &AFPSPlaygroundCharacter::ReleaseADS);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSPlaygroundCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSPlaygroundCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPSPlaygroundCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPSPlaygroundCharacter::LookUpAtRate);
}






/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVEMENT
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AFPSPlaygroundCharacter::MoveForward(float Value)
{

	MoveForwardAxis = Value;

	if (Value != 0.0f)
	{
		// Add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPSPlaygroundCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// Add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSPlaygroundCharacter::TurnAtRate(float Rate)
{
	// Calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPSPlaygroundCharacter::LookUpAtRate(float Rate)
{
	// Calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFPSPlaygroundCharacter::StartCrouch()
{
	Crouch();
}

void AFPSPlaygroundCharacter::StopCrouch()
{
	UnCrouch();
}






/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FIRING
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AFPSPlaygroundCharacter::PullTrigger()
{
	SMG->OnFireSMG();
}

void AFPSPlaygroundCharacter::ReleaseTrigger()
{
	SMG->StopFireSMG();
}

void AFPSPlaygroundCharacter::OnFire(FRotator BulletRotation)
{
	Server_OnFireSMG(MuzzleLocation->GetComponentRotation(), BulletRotation, MuzzleLocation->GetComponentLocation());
}

void AFPSPlaygroundCharacter::FireSoundSMG(USoundBase* Sound)
{
	Server_FireSoundSMG(Sound);
}


//////////////// RPCs START ////////////////////


bool AFPSPlaygroundCharacter::Server_OnFireSMG_Validate(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn)
{
	return true;
}

void AFPSPlaygroundCharacter::Server_OnFireSMG_Implementation(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn)
{
	// Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	// Spawn the projectile at the muzzle
	if (BulletBlueprint != NULL)
	{
		GetWorld()->SpawnActor<AFPSPlaygroundProjectile>(BulletBlueprint, MuzzleLocationSpawn, MuzzleRotation + BulletRotation, ActorSpawnParams);
	}
}

bool AFPSPlaygroundCharacter::Server_FireSoundSMG_Validate(USoundBase* Sound)
{
	return true;
}

void AFPSPlaygroundCharacter::Server_FireSoundSMG_Implementation(USoundBase* Sound)
{
	UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation(), 0.5f, 0.7f);
}

/////////////////// RPCs END //////////////////////



void AFPSPlaygroundCharacter::FireAnimationSMG(UAnimMontage* FireAnimation1P)
{
	Mesh1PAnimInstance->Montage_Play(FireAnimation1P, 1.f);

	if (Mesh3PAnimInstance != nullptr)
	{
		Mesh3PAnimInstance->Montage_Play(SMGFireAnimation3P, 1.0f);
	}
}

void AFPSPlaygroundCharacter::OnADS()
{
	bIsADS = true;
	UpdateADSFOV();
}

void AFPSPlaygroundCharacter::ReleaseADS()
{
	bIsADS = false;
	UpdateADSFOV();
}





/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ON TAKE DAMAGE 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float AFPSPlaygroundCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (Health <= 0.f)
	{
		return 0.f;
	}

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}

	return ActualDamage;
}


bool AFPSPlaygroundCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| Role != ROLE_Authority						// not authority
		|| GetWorld()->GetAuthGameMode<AFPSPlaygroundGameMode>() == NULL
		|| GetWorld()->GetAuthGameMode<AFPSPlaygroundGameMode>()->GetMatchState() == MatchState::LeavingMap)	// level transition occurring
	{
		return false;
	}

	return true;
}


bool AFPSPlaygroundCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Min(0.0f, Health);

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	NetUpdateFrequency = GetDefault<AFPSPlaygroundCharacter>()->NetUpdateFrequency;
	GetCharacterMovement()->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}


void AFPSPlaygroundCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}

	bReplicateMovement = false;
	TearOff();
	bIsDying = true;

	if (Role == ROLE_Authority)
	{
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);
	}

	DetachFromControllerPendingDestroy();

	SetActorEnableCollision(true);

	// disable collisions on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AFPSPlaygroundCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (Role == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);
	}

	if (DamageTaken > 0.f)
	{
		ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
	}
}

void AFPSPlaygroundCharacter::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;
}

void AFPSPlaygroundCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
}

void AFPSPlaygroundCharacter::BeginDestroy()
{
	Super::BeginDestroy();

}


/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GETTERS
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AFPSPlaygroundCharacter::GetIsMoving()
{
	if (GetCharacterMovement()->Velocity.Size() > 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

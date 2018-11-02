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
#include "SMG.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFPSPlaygroundCharacter

void AFPSPlaygroundCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AFPSPlaygroundCharacter, bIsADS);
}

AFPSPlaygroundCharacter::AFPSPlaygroundCharacter()
{
	bReplicates = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	RootComponent = GetCapsuleComponent();

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->bOwnerNoSee = true;
	Mesh3P->bCastDynamicShadow = false;
	Mesh3P->CastShadow = false;
	Mesh3P->SetupAttachment(GetCapsuleComponent());

	GunMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh3P"));
	GunMesh3P->bOwnerNoSee = true;
	GunMesh3P->bCastDynamicShadow = false;
	GunMesh3P->CastShadow = false;
	GunMesh3P->SetupAttachment(Mesh3P);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(FirstPersonCameraComponent);
}

void AFPSPlaygroundCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (SMGBlueprint == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("SMG blueprint missing."));
		return;
	}

	SMG = GetWorld()->SpawnActor<ASMG>(SMGBlueprint);

	if (SMG != nullptr)
	{
		SMG->SetOwner(this);
		SMG->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	}

	GunMesh3P->AttachToComponent(Mesh3P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	Mesh1PAnimInstance = Mesh1P->GetAnimInstance();

	SpawnedWeapon();
}

void AFPSPlaygroundCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFPSPlaygroundCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSPlaygroundCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFPSPlaygroundCharacter::StopCrouch);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSPlaygroundCharacter::PullTrigger);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPSPlaygroundCharacter::ReleaseTrigger);

	PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &AFPSPlaygroundCharacter::OnADS);
	PlayerInputComponent->BindAction("ADS", IE_Released, this, &AFPSPlaygroundCharacter::ReleaseADS);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFPSPlaygroundCharacter::OnSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFPSPlaygroundCharacter::StopSprint);

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

void AFPSPlaygroundCharacter::MoveForward(float Value)
{

	MoveForwardAxis = Value;

	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}

	if (Value > 0.0f)
	{
		if (!bIsCrouched)
		{
			if (bOnSprint)
			{
				OnSprint();
				//bCanShoot = false;
				ReleaseTrigger();
			}

			bIsWalkingForward = true;
		}
	}
	else
	{
		if (bIsSprinting)
		{
			SetStopSprint();
			//bCanShoot = true;
		}

		bIsWalkingForward = false;
	}
}

void AFPSPlaygroundCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSPlaygroundCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPSPlaygroundCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

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

bool AFPSPlaygroundCharacter::Server_OnFireSMG_Validate(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn)
{
	return true;
}

void AFPSPlaygroundCharacter::Server_OnFireSMG_Implementation(FRotator MuzzleRotation, FRotator BulletRotation, FVector MuzzleLocationSpawn)
{
	//Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	// spawn the projectile at the muzzle
	if (ProjectileClass != NULL)
	{
		GetWorld()->SpawnActor<AFPSPlaygroundProjectile>(ProjectileClass, MuzzleLocationSpawn, MuzzleRotation + BulletRotation, ActorSpawnParams);
	}

}

bool AFPSPlaygroundCharacter::Server_PlayRecoilAnimationAndSoundSMG_Validate(AActor* Actor, USoundBase* Sound, FVector Location)
{
	return true;
}

void AFPSPlaygroundCharacter::Server_PlayRecoilAnimationAndSoundSMG_Implementation(AActor* Actor, USoundBase* Sound, FVector Location)
{
	//AnimInstance->Montage_Play(RecoilAnim, 1.f);
	UGameplayStatics::PlaySoundAtLocation(Actor, Sound, Location, 0.5f, 0.7f);
}

void AFPSPlaygroundCharacter::OnADS()
{
	if (!bIsSprinting)
	{
		bIsADS = true;
	}
}

void AFPSPlaygroundCharacter::ReleaseADS()
{
	bIsADS = false;
}

void AFPSPlaygroundCharacter::StartCrouch()
{
	Crouch();

	bIsSprinting = false;
	StopSprint();
	//bCanShoot = true;
}

void AFPSPlaygroundCharacter::StopCrouch()
{
	UnCrouch();

	if (bOnSprint)
	{
		OnSprint();
	}
}

bool AFPSPlaygroundCharacter::GetIsADS()
{
	return bIsADS;
}

bool AFPSPlaygroundCharacter::GetIsCrouched()
{
	return bIsCrouched;
}

bool AFPSPlaygroundCharacter::GetIsMoving()
{
	return bIsMoving;
}

void AFPSPlaygroundCharacter::OnSprint()
{
	bOnSprint = true;

	if (!bIsCrouched)
	{
		if (MoveForwardAxis > 0.0f)
		{
			SetOnSprint();
			bIsSprinting = true;
			//bCanShoot = false;

			if (MoveForwardAxis <= 0.0f)
			{
				StopSprint();
			}
		}
	}
}

void AFPSPlaygroundCharacter::StopSprint()
{
	bOnSprint = false;
	SetStopSprint();

	if (bIsSprinting)
	{
		bIsSprinting = false;
		//bCanShoot = true;
	}
}

void AFPSPlaygroundCharacter::RecieveIsMoving(bool IsMoving)
{
	bIsMoving = IsMoving;
}

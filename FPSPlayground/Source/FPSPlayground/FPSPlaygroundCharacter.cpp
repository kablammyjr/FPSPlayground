// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FPSPlaygroundCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "SMG.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFPSPlaygroundCharacter

AFPSPlaygroundCharacter::AFPSPlaygroundCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

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
}

void AFPSPlaygroundCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (SMGBlueprint == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Gun blueprint missing."));
		return;
	}

	SMG = GetWorld()->SpawnActor<ASMG>(SMGBlueprint);

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	SMG->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
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
				bCanFireGun = false;
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
			bCanFireGun = true;
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
	if (bIsSprinting)
	{
		StopSprint();
		PullTrigger();
	}

	if (bCanFireGun)
	{
		SMG->OnFire();
		bIsFiring = true;
	}
}

void AFPSPlaygroundCharacter::ReleaseTrigger()
{
	SMG->OnRelease();
	bIsFiring = false;
}

bool AFPSPlaygroundCharacter::GetIsFiring()
{
	return bIsFiring;
}

void AFPSPlaygroundCharacter::PlayRecoilAnimation()
{
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			if (bCanRecoil)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);

				bCanRecoil = false;

				FTimerHandle FuzeTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &AFPSPlaygroundCharacter::CanRecoil, 0.1f, false);
			}
		}
	}
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
	bCanFireGun = true;
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

void AFPSPlaygroundCharacter::CanRecoil()
{
	bCanRecoil = true;
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
			bCanFireGun = false;

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
		bCanFireGun = true;
	}
}

bool AFPSPlaygroundCharacter::GetCanFireGun()
{
	return bCanFireGun;
}

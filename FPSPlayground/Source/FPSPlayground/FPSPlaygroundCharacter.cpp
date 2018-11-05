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



/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OVERRIDES 
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void AFPSPlaygroundCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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



	/////////////// COMPONENTS //////////////////

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

	// SMG spawn and setup
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
	Mesh3PAnimInstance = Mesh3P->GetAnimInstance();

	SpawnedWeapon();
}

void AFPSPlaygroundCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSPlaygroundCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFPSPlaygroundCharacter::StopCrouch);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSPlaygroundCharacter::PullTrigger);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPSPlaygroundCharacter::ReleaseTrigger);

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
	if (ProjectileClass != NULL)
	{
		GetWorld()->SpawnActor<AFPSPlaygroundProjectile>(ProjectileClass, MuzzleLocationSpawn, MuzzleRotation + BulletRotation, ActorSpawnParams);
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
}

void AFPSPlaygroundCharacter::ReleaseADS()
{
	bIsADS = false;
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

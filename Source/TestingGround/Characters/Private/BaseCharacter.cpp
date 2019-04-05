// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "../Public/BaseCharacter.h"
#include "../../Weapons/Public/GunActor.h"
#include "AIController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"

//////////////////////////////////////////////////////////////////////////
// ABaseCharacter

ABaseCharacter::ABaseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// FP Camera
	FP_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FP_Camera"));
	FP_Camera->SetupAttachment(GetCapsuleComponent());
	FP_Camera->RelativeLocation = FVector(5.44f, 7.75f, 69.f); // Position the camera
	FP_Camera->bUsePawnControlRotation = true; // Rotate the camera based on the controller

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh_Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh_Arms->SetOnlyOwnerSee(true);
	Mesh_Arms->SetupAttachment(FP_Camera);
	Mesh_Arms->bCastDynamicShadow = false;
	Mesh_Arms->CastShadow = false;
	Mesh_Arms->RelativeRotation = FRotator(0.f, 0.f, 0.f);
	Mesh_Arms->RelativeLocation = FVector(3.5f, -4.4f, -160.7f);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ABaseCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (WeaponClass == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Missing weapon blueprint"));
		return;
	}

	//Attach weapon mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	Weapon = GetWorld()->SpawnActor<AGunActor>(WeaponClass);
	if (Cast<AAIController>(GetController())) {
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	}
	else {
		Weapon->AttachToComponent(Mesh_Arms, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	}
	Weapon->AnimInstance = Mesh_Arms->GetAnimInstance();
} 

//////////////////////////////////////////////////////////////////////////
// Input

void ABaseCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABaseCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABaseCharacter::LookUpAtRate);
}

AGunActor* ABaseCharacter::GetWeapon()
{
	return Weapon;
}

void ABaseCharacter::AttemptFire()
{
	if (Weapon == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Weapon Missing."));
		return;
	}

	Weapon->OnFire();
}

void ABaseCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

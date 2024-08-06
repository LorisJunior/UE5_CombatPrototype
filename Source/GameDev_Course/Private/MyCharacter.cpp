// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "Gameframework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include <EnhancedInputSubsystems.h>
#include "Weapon.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	// Camera Setup

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(FName("View Camera"));
	ViewCamera->SetupAttachment(CameraBoom);
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		Input->BindAction(EquipAction, ETriggerEvent::Triggered, this, &AMyCharacter::Equip);
		Input->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMyCharacter::Attack);
		Input->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find enhanced input"));
	}
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyCharacter::GetHit_Implementation(const FVector& ImpactOrigin, const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactOrigin, ImpactPoint);

	ActionState = EActionState::EAS_HitReaction;
	SetWeaponTraceDisabled();
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	Tags.Add(FName("Player"));
}

void AMyCharacter::Attack()
{
	if (CanAttack())
	{
		if (ActionState == EActionState::EAS_Attacking)
		{
			canCombo = true;
		}
		else
		{
			ActionState = EActionState::EAS_Attacking;
			PlayAttackMontage();
		}
	}
}

bool AMyCharacter::CanAttack()
{
	return GetAttackMontage() && CharacterState == ECharacterStates::ECS_Equipped && ActionState != EActionState::EAS_HitReaction;
}

void AMyCharacter::PlayAttackMontage()
{
	int32 MaxSectionIndex = AttackMontageSections.Num() - 1;

	if (AttackSectionIndex < MaxSectionIndex)
	{
		AttackSectionIndex++;
	}
	else
	{
		AttackSectionIndex = 0;
	}

	PlayMontageSection(GetAttackMontage(), AttackMontageSections[AttackSectionIndex]);
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D Movement = Value.Get<FVector2D>();

	// Z axis rotation

	const FRotator YawRotation(0, GetControlRotation().Yaw, 0);

	// Get forward and right direction vector

	FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// Add movement

	AddMovementInput(ForwardDirection, Movement.X);
	AddMovementInput(RightDirection, Movement.Y);
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookDirection = Value.Get<FVector2D>();

	AddControllerYawInput(LookDirection.X);
	AddControllerPitchInput(LookDirection.Y);
}

void AMyCharacter::Equip(const FInputActionValue& Value)
{
	AWeapon* OverlapingWeapon = Cast<AWeapon>(OverlapingItem);

	if (CanEquipWeapon(OverlapingWeapon))
	{
		GrabWeapon(OverlapingWeapon);
	}
	else if (CanAttachWeaponToBack())
	{
		PlayAnimMontage(EquipMontage, 1.0f, FName("Disarm"));
	}
	else if (CanUseWeaponFromBack())
	{
		PlayAnimMontage(EquipMontage, 1.0f, FName("Equip"));
	}
}

bool AMyCharacter::CanUseWeaponFromBack()
{
	return EquippedWeapon && CharacterState == ECharacterStates::ECS_Unequipped;
}

bool AMyCharacter::CanAttachWeaponToBack()
{
	return EquippedWeapon && CharacterState == ECharacterStates::ECS_Equipped && ActionState == EActionState::EAS_Unoccupied;
}

bool AMyCharacter::CanEquipWeapon(AWeapon* Weapon)
{
	return Weapon && CharacterState == ECharacterStates::ECS_Unequipped;
}

void AMyCharacter::GrabWeapon(AWeapon* Weapon)
{
	CharacterState = ECharacterStates::ECS_Equipped;
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	EquippedWeapon = Weapon;
}

void AMyCharacter::StartCombo()
{
	if (canCombo)
	{
		canCombo = false;
		PlayAttackMontage();
	}
}

void AMyCharacter::ResetCombo()
{
	AttackSectionIndex = -1;
	ActionState = EActionState::EAS_Unoccupied;
	canCombo = false;
}

void AMyCharacter::AttachToHand()
{
	EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("RightHandSocket"));
	CharacterState = ECharacterStates::ECS_Equipped;
}

void AMyCharacter::AttachToBack()
{
	EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("SpineSocket"));
	CharacterState = ECharacterStates::ECS_Unequipped;
}

void AMyCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

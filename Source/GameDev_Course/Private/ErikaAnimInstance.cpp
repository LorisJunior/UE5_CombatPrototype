// Fill out your copyright notice in the Description page of Project Settings.


#include "ErikaAnimInstance.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UErikaAnimInstance::NativeInitializeAnimation()
{
	Character = Cast<AMyCharacter>(TryGetPawnOwner());

	if (Character)
	{
		CharacterMovement = Character->GetCharacterMovement();
	}
}

void UErikaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (Character)
	{
		CharacterStates = Character->GetCharacterStates();
	}

	if (CharacterMovement)
	{
		GroundSpeed = CharacterMovement->Velocity.Size2D();
		isInAir = CharacterMovement->IsFalling();
	}
}

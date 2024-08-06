// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "ErikaAnimInstance.generated.h"

class UCharacterMovementComponent;
class AMyCharacter;

/**
 * 
 */
UCLASS()
class GAMEDEV_COURSE_API UErikaAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	// Override default functions

	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;

	// Get character movement
	UCharacterMovementComponent* CharacterMovement;

protected:
	UPROPERTY(BlueprintReadOnly)
	AMyCharacter* Character;

public:

	// Character animBP variables

	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly)
	bool isInAir;
 
	UPROPERTY(BlueprintReadWrite)
	ECharacterStates CharacterStates;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "MyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AItem;

UCLASS()
class GAMEDEV_COURSE_API AMyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AMyCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactOrigin, const FVector& ImpactPoint) override;

protected:

	virtual void BeginPlay() override;

	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void PlayAttackMontage() override;

private:

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	bool CanUseWeaponFromBack();
	bool CanAttachWeaponToBack();
	bool CanEquipWeapon(AWeapon* Weapon);
	void GrabWeapon(AWeapon* Weapon);

	UFUNCTION(BlueprintCallable)
	void StartCombo();

	UFUNCTION(BlueprintCallable)
	void ResetCombo();

	UFUNCTION(BlueprintCallable)
	void AttachToHand();

	UFUNCTION(BlueprintCallable)
	void AttachToBack();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* EquipAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AttackAction = nullptr;

	ECharacterStates CharacterState = ECharacterStates::ECS_Unequipped;

	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlapingItem = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* EquipMontage = nullptr;

	int32 AttackSectionIndex = -1;

	bool canCombo = false;

public:
	FORCEINLINE void SetOverlapingItem(AItem* Item) { OverlapingItem = Item; }
	FORCEINLINE ECharacterStates GetCharacterStates() const { return CharacterState; }
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;

UCLASS()
class GAMEDEV_COURSE_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactOrigin, const FVector& ImpactPoint) override;

	virtual bool CanAttack();
	virtual void Attack();
	virtual void LockTarget(AActor* Target);
	virtual void UnlockTarget();
	virtual void HandleDamage(float DamageAmount);
	virtual void Die();
	virtual bool IsAlive();
	void DirectionalHitReact(const FVector& ImpactPoint);
	void DisableCapsule();
	float GetTargetDistance(AActor* Target);


	virtual void PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	void PlayHitSound(const FVector& ImpactPoint);
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	void SpawnParticles(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	void SetWeaponTraceEnabled();

	UFUNCTION(BlueprintCallable)
	void SetWeaponTraceDisabled();
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon = nullptr;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

private:


	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AttackMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HitReactMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticles;

public:

	FORCEINLINE UAnimMontage* GetAttackMontage() const { return AttackMontage; };
};

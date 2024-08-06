// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class AAIController; 
class UPawnSensingComponent;
class AWeapon;

UCLASS()
class GAMEDEV_COURSE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	virtual void GetHit_Implementation(const FVector& ImpactOrigin, const FVector& ImpactPoint) override;

protected:

	virtual void BeginPlay() override;
	virtual bool CanAttack() override;
	virtual void Attack() override;
	virtual void LockTarget(AActor* Target) override;
	virtual void UnlockTarget() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void Die() override;
	virtual int32 PlayDeathMontage() override;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:

	// AI Behaviour

	void InitializeEnemy();
	void SpawnDefaultWeapon();
	void SetHealthBarVisiblity(bool Visibility);
	void CheckPatrolTarget();
	void CheckCombatTarget();
	bool InTargetRange(AActor* Target, double Radius);
	void LoseInterest();
	void ChaseTarget();
	void StartPatrolling();
	AActor* ChoosePatrolTarget();
	void MoveToTarget(AActor* Target);
	void ClearPatrolTimer();
	void PatrolTimerFinished();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsAttacking();
	bool IsChasing();
	bool IsDead();
	bool IsEngaged();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION()
	void PawnSeen(APawn* Pawn); // OnSeePawn Delegate

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> RootMotionAttackMontageSections;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double LongAttackRadius = 400.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double CloseAttackRadius = 200.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRate = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 120.f;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;
};

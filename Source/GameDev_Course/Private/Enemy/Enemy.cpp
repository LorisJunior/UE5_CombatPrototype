// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AttributeComponent.h"
#include "HealthBarComponent.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Character Mesh

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	// Characte Movement

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// HUD

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(FName("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	// PawnSensing

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(FName("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead())
		return;

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	ChaseTarget();
	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactOrigin, const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactOrigin, ImpactPoint);
	if (!IsDead())
		SetHealthBarVisiblity(true);
	ClearPatrolTimer();
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing) 
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	InitializeEnemy();
}

bool AEnemy::CanAttack()
{
	return IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsDead();
}

void AEnemy::Attack()
{
	EnemyState = EEnemyState::EES_Engaged;

	float TargetDistance = GetTargetDistance(CombatTarget);
	if (!RootMotionAttackMontageSections.IsEmpty() && TargetDistance <= LongAttackRadius && TargetDistance > CloseAttackRadius)
	{
		PlayRandomMontageSection(GetAttackMontage(), RootMotionAttackMontageSections);
	}
	else
	{
		PlayAttackMontage();
	}

}

void AEnemy::LockTarget(AActor* Target)
{
	bUseControllerRotationYaw = true;
	EnemyController->SetFocus(Target);
}

void AEnemy::UnlockTarget()
{
	bUseControllerRotationYaw = false;
	EnemyController->ClearFocus(EAIFocusPriority::LastFocusPriority);
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::Die()
{
	EnemyState = EEnemyState::EES_Dead;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ClearAttackTimer();
	PlayDeathMontage();
	DisableCapsule();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetHealthBarVisiblity(false);
	SetLifeSpan(3.f);
	SetWeaponTraceDisabled();
}

int32 AEnemy::PlayDeathMontage()
{
	int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);

	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();

	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::InitializeEnemy()
{
	Tags.Add(FName("Enemy"));
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	SetHealthBarVisiblity(false);
	SpawnDefaultWeapon();
}

void AEnemy::SetHealthBarVisiblity(bool Visibility)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(Visibility);
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, 5.f);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged())
			StartPatrolling();

	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if (!IsEngaged())
			ChaseTarget();
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr)
		return false;

	DrawDebugSphere(GetWorld(), Target->GetActorLocation(), 25.f, 12, FColor::Red);
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	return DistanceToTarget <= Radius;
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	SetHealthBarVisiblity(false);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr)
		return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(60.f);
	EnemyController->MoveTo(MoveRequest);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	LockTarget(CombatTarget);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackRate);
}

void AEnemy::ClearAttackTimer()
{
	UnlockTarget();
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, LongAttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, LongAttackRadius);
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::PawnSeen(APawn* Pawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead && 
		EnemyState != EEnemyState::EES_Chasing && 
		EnemyState < EEnemyState::EES_Attacking && 
		Pawn->ActorHasTag(FName("Player"));

	if (bShouldChaseTarget)
	{
		CombatTarget = Pawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

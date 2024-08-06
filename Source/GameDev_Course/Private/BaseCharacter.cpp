// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Weapon.h"
#include "AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Custom Attributes

	Attributes = CreateDefaultSubobject<UAttributeComponent>(FName("Attributes"));

	// Capsule
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactOrigin, const FVector& ImpactPoint)
{
	if (IsAlive())
	{
		DirectionalHitReact(ImpactOrigin);
	}
	else
	{
		Die();
	}

	PlayHitSound(ImpactPoint);
	SpawnParticles(ImpactPoint);
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::LockTarget(AActor* Target)
{
}

void ABaseCharacter::UnlockTarget()
{
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::Die()
{
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::PlayAttackMontage()
{
	PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	if (Montage)
	{
		PlayAnimMontage(Montage, 1.0f, SectionName);
	}
}

void ABaseCharacter::SpawnParticles(const FVector& ImpactPoint)
{
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}
}

void ABaseCharacter::SetWeaponTraceEnabled()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StartBoxTraceTimer();
	}
}

void ABaseCharacter::SetWeaponTraceDisabled()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->ClearBoxTraceTimer();
		EquippedWeapon->ClearIgnoreActors();
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	// Impact Direction

	FVector ForwardDirection = GetActorForwardVector();

	FVector ImpactFlattened = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);

	FVector ImpactDirection = (ImpactFlattened - GetActorLocation()).GetSafeNormal();

	// A . B = |A| * |B| * cos(theta)

	const double CosTheta = FVector::DotProduct(ForwardDirection, ImpactDirection);

	double ArcDegrees = FMath::Acos(CosTheta) * FMathf::RadToDeg;

	const FVector CrossProduct = FVector::CrossProduct(ForwardDirection, ImpactDirection);

	// If Cross Product points down ArcDegress is negative

	if (CrossProduct.Z < 0.f)
	{
		ArcDegrees *= -1;
	}

	// Move the Enemy in the Hit Direction

	if (HitReactMontage)
	{
		FName SectionName = "FromBack";

		if (ArcDegrees >= -45.f && ArcDegrees < 45.f)
		{
			SectionName = "FromFront";
		}
		if (ArcDegrees >= -135.f && ArcDegrees < -45.f)
		{
			SectionName = "FromLeft";
		}
		if (ArcDegrees >= 45.f && ArcDegrees < 135.f)
		{
			SectionName = "FromRight";
		}

		PlayAnimMontage(HitReactMontage, 1.0f, FName(SectionName));
	}
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float ABaseCharacter::GetTargetDistance(AActor* Target)
{
	return (Target->GetActorLocation() - GetActorLocation()).Size();
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0)
		return - 1;

	int32 MaxSectionIndex = SectionNames.Num() - 1;
	int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);

	return Selection;
}


// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "HitInterface.h"

AWeapon::AWeapon()
{
	// Box Start

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(FName("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	// Box End

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(FName("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::StartBoxTraceTimer()
{
	UKismetSystemLibrary::K2_SetTimer(this, FString("CheckHit"), 0.001f, true);
}

void AWeapon::ClearBoxTraceTimer()
{
	UKismetSystemLibrary::K2_ClearTimer(this, FString("CheckHit"));
}

void AWeapon::ClearIgnoreActors()
{
	IgnoreActors.Empty();
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator )
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	SetCanHover(false);
	AttachToComponent(InParent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), InSocketName);
	DisableSphereCollision();
	DisableEmbersEffect();
}

void AWeapon::CheckHit()
{
	FHitResult BoxHit;

	BoxTrace(BoxHit);

	if (BoxHit.GetActor())
	{
		if (ActorHasSameTag(BoxHit))
			return;

		HitActor(BoxHit);
	}
}

bool AWeapon::ActorHasSameTag(FHitResult& BoxHit)
{
	return GetOwner()->ActorHasTag("Enemy") && BoxHit.GetActor()->ActorHasTag("Enemy");
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	IgnoreActors.AddUnique(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxSize, GetActorRotation(), ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, bDrawDebugBox ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, BoxHit, true, FLinearColor::Red, FLinearColor::Green, 1.0f);
	IgnoreActors.AddUnique(BoxHit.GetActor());
}

void AWeapon::HitActor(FHitResult& BoxHit)
{
	ApplyDamage(BoxHit.GetActor());
	ExecuteGetHit(BoxHit);
	CreateFields(BoxHit.ImpactPoint);
}

void AWeapon::ApplyDamage(AActor* DamagedActor)
{
	UGameplayStatics::ApplyDamage(DamagedActor, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitActor = Cast<IHitInterface>(BoxHit.GetActor());

	if (HitActor)
	{
		HitActor->Execute_GetHit(BoxHit.GetActor(), GetOwner()->GetActorLocation(), BoxHit.ImpactPoint);
	}
}

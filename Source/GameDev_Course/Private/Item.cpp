// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include "MyCharacter.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize Item

	Item = CreateDefaultSubobject<UStaticMeshComponent>(FName("Item"));
	Item->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Item->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Item;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(FName("SphereCollision"));
	SphereCollision->SetupAttachment(Item);

	EmbersEffect = CreateDefaultSubobject<UNiagaraComponent>(FName("Embers"));
	EmbersEffect->SetupAttachment(GetRootComponent());
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (canHover)
	{
		Hover(DeltaTime);
	}
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AItem::SphereCollisionBeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AItem::SphereCollisionEndOverlap);
}

void AItem::DisableSphereCollision()
{
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItem::DisableEmbersEffect()
{
	EmbersEffect->Deactivate();
}

void AItem::SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMyCharacter* Character = Cast<AMyCharacter>(OtherActor))
	{
		Character->SetOverlapingItem(this);
	}
}

void AItem::SphereCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMyCharacter* Character = Cast<AMyCharacter>(OtherActor))
	{
		Character->SetOverlapingItem(nullptr);
	}
}

void AItem::Hover(float DeltaTime)
{
	RunningTime += DeltaTime;
	float ZValue = Amplitude * FMath::Sin(RunningTime * HoveringSpeed);

	AddActorWorldOffset(FVector(0, 0, ZValue));
}
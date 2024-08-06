// Fill out your copyright notice in the Description page of Project Settings.


#include "Treasure.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMyCharacter* Character = Cast<AMyCharacter>(OtherActor))
	{
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}

		Destroy();
	}
}

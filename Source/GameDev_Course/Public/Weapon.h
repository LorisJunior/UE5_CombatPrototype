// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

class UBoxComponent;
class USceneComponent;

UCLASS()
class GAMEDEV_COURSE_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void StartBoxTraceTimer();
	void ClearBoxTraceTimer();
	void ClearIgnoreActors();

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:

	UFUNCTION()
	void CheckHit(); // UKismetSystemLibrary::K2_SetTimer Delegate

	bool ActorHasSameTag(FHitResult& BoxHit);

	void BoxTrace(FHitResult& BoxHit);
	void HitActor(FHitResult& BoxHit);
	void ApplyDamage(AActor* DamagedActor);
	void ExecuteGetHit(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxSize = FVector(5.0f, 40.0f, 5.0f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bDrawDebugBox = false;

	TArray<AActor*> IgnoreActors;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class UNiagaraComponent;

UCLASS()
class GAMEDEV_COURSE_API AItem : public AActor
{
	GENERATED_BODY()

public:

	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void BeginPlay() override;

	void DisableSphereCollision();
	void DisableEmbersEffect();

	UFUNCTION()
	virtual void SphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void SphereCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	void Hover(float DeltaTime);

	bool canHover = true;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Item;

	UPROPERTY(VisibleAnywhere)
	float RunningTime;

	UPROPERTY(EditInstanceOnly)
	float Amplitude = 2.0f;

	UPROPERTY(EditInstanceOnly)
	float HoveringSpeed = 5.0f;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* EmbersEffect;
	
public:
	
	FORCEINLINE void SetCanHover(bool hover) { canHover = hover; }
};

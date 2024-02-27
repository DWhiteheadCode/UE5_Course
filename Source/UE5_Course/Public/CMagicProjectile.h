// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CBaseProjectile.h"

#include "GameplayTagContainer.h"

#include "CMagicProjectile.generated.h"

class UAudioComponent;
class USoundCue;

UCLASS()
class UE5_COURSE_API ACMagicProjectile : public ACBaseProjectile
{
	GENERATED_BODY()

public:
	ACMagicProjectile();

	void PostInitializeComponents() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage;

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	FGameplayTag ParryTag;
};

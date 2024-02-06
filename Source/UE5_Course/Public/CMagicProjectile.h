// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CBaseProjectile.h"

#include "CMagicProjectile.generated.h"

class UAudioComponent;
class USoundCue;

UCLASS()
class UE5_COURSE_API ACMagicProjectile : public ACBaseProjectile
{
	GENERATED_BODY()

public:
	void PostInitializeComponents() override;

protected:
	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

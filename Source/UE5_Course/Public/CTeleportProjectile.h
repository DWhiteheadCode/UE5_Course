// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CBaseProjectile.h"
#include "CTeleportProjectile.generated.h"

class UParticleSystem;

/**
 * 
 */
UCLASS()
class UE5_COURSE_API ACTeleportProjectile : public ACBaseProjectile
{
	GENERATED_BODY()
	
public:
	ACTeleportProjectile();

protected:
	void BeginPlay() override;

	void Detonate() override;

	UFUNCTION()
	void Teleport();

	UPROPERTY(VisibleAnywhere, Category = Timers)
	float DetonateDelay = 0.2f;

	UPROPERTY(VisibleAnywhere, Category = Timers)
	float TeleportDelay = 0.2f;

	FTimerHandle TimerHandle_TeleportProjectileDetonation;
	FTimerHandle TimerHandle_TeleportProjectileTeleportation;
};

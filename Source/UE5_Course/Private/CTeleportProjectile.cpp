// Fill out your copyright notice in the Description page of Project Settings.


#include "CTeleportProjectile.h"

#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


ACTeleportProjectile::ACTeleportProjectile()
{
	DetonateDelay = 0.2f;
	TeleportDelay = 0.2f;

	MovementSpeed = 3000.f;
	MovementComp->InitialSpeed = MovementSpeed;
}

void ACTeleportProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_TeleportProjectileDetonation, this, &ACTeleportProjectile::Detonate, DetonateDelay);
}

void ACTeleportProjectile::Detonate() 
{
	// Prevent timer detonation if collision caused early detonation
	GetWorldTimerManager().ClearTimer(TimerHandle_TeleportProjectileDetonation);

	// Spawn impact emitter as Super not called as we don't want to call Destroy() yet
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	EffectComp->DeactivateSystem();

	GetWorldTimerManager().SetTimer(TimerHandle_TeleportProjectileTeleportation, this, &ACTeleportProjectile::Teleport, TeleportDelay);

	MovementComp->StopMovementImmediately();
}

void ACTeleportProjectile::Teleport()
{
	if (AActor* InstigatorPtr = GetInstigator())
	{
		InstigatorPtr->TeleportTo(GetActorLocation(), InstigatorPtr->GetActorRotation());
	}

	this->Destroy();
}
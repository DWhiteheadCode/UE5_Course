// Fill out your copyright notice in the Description page of Project Settings.


#include "CMagicProjectile.h"

#include "Components/SphereComponent.h"
#include "CAttributeComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "CGameplayFunctionLibrary.h"

ACMagicProjectile::ACMagicProjectile()
{
	Damage = 20.0f;
}

void ACMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ACMagicProjectile::OnActorOverlap);
}


void ACMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator()) // Make sure an actor was hit
	{	
		if (UCGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, Damage, SweepResult))
		{
			Detonate();
		}
	}
}


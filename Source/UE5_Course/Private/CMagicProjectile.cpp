// Fill out your copyright notice in the Description page of Project Settings.


#include "CMagicProjectile.h"

#include "Components/SphereComponent.h"
#include "CAttributeComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "CGameplayFunctionLibrary.h"
#include "CActionComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CActionEffect.h"

ACMagicProjectile::ACMagicProjectile()
{
	DamageAmount = 20.0f;

	InitialLifeSpan = 10.0f;
}

void ACMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ACMagicProjectile::OnActorOverlap);
}


void ACMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator()) 
	{	
		// Check if the hit was parried
		UCActionComponent* ActionComp = Cast<UCActionComponent>(OtherActor->GetComponentByClass(UCActionComponent::StaticClass()));
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag)) 
		{
			MovementComp->Velocity = -MovementComp->Velocity;
			SetInstigator(Cast<APawn>(OtherActor));

			return;
		}


		if (UCGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			Detonate();

			if (ActionComp && HasAuthority())
			{
				ActionComp->AddAction( BurningActionClass, GetInstigator() );
			}
		}
	}
}


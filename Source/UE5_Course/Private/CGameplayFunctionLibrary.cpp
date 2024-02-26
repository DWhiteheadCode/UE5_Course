// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameplayFunctionLibrary.h"

#include "CAttributeComponent.h"

bool UCGameplayFunctionLibrary::ApplyDamage(AActor* InstigatorActor, AActor* TargetActor, float DamageAmount)
{
	if (TargetActor)
	{
		if (UCAttributeComponent* TargetAttributeComp = UCAttributeComponent::GetAttributeComponent(TargetActor))
		{
			return TargetAttributeComp->ApplyHealthChange(InstigatorActor, - DamageAmount);
		}
	}

	return false;
}

bool UCGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* InstigatorActor, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
	if (ApplyDamage(InstigatorActor, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();

		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();

			float ImpulseAmount = 300000.f;
			HitComp->AddImpulseAtLocation( Direction * ImpulseAmount, HitResult.ImpactPoint, HitResult.BoneName );
		}

		return true;
	}

	return false;
}

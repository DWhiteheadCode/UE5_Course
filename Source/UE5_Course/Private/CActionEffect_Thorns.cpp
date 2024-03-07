// Fill out your copyright notice in the Description page of Project Settings.


#include "CActionEffect_Thorns.h"

#include "CAttributeComponent.h"
#include "CActionComponent.h"
#include "CGameplayFunctionLibrary.h"
#include "GenericPlatform/GenericPlatformMath.h"


UCActionEffect_Thorns::UCActionEffect_Thorns()
{
	Duration = 0.0f;
	Period = 0.0f;
	
	DamageFraction = 0.2f;
	ActionName = "Thorns";
}

void UCActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	UCAttributeComponent* OwnerAttributeComp = UCAttributeComponent::GetAttributeComponent(GetOwningComponent()->GetOwner());

	if (ensure(OwnerAttributeComp))
	{
		OwnerAttributeComp->OnHealthChanged.AddDynamic(this, &UCActionEffect_Thorns::OnOwnerHealthChanged);
	}
}

void UCActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	UCAttributeComponent* OwnerAttributeComp = UCAttributeComponent::GetAttributeComponent(GetOwningComponent()->GetOwner());

	if (ensure(OwnerAttributeComp))
	{
		OwnerAttributeComp->OnHealthChanged.RemoveDynamic(this, &UCActionEffect_Thorns::OnOwnerHealthChanged);
	}
}

void UCActionEffect_Thorns::OnOwnerHealthChanged(AActor* InstigatorActor, UCAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	// Ignore healing
	if (Delta > 0.0f)
	{
		return;
	}

	AActor* OwnerActor = GetOwningComponent()->GetOwner();
	
	if (ensure(OwnerActor))
	{
		// Ignore self damage
		if (OwnerActor == InstigatorActor)
		{
			return;
		}

		if (ensure(OwningComp))
		{
			// Delta is negative. Convert to positive damage
			float ThornDamage = - (FGenericPlatformMath::RoundToInt(Delta * DamageFraction));

			// Note: Dealing damage TO Instigator, which is instigated BY Owner
			UCGameplayFunctionLibrary::ApplyDamage(OwnerActor, InstigatorActor, ThornDamage);
		}
	}	
}
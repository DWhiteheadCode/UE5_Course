// Fill out your copyright notice in the Description page of Project Settings.


#include "CHealthPickup.h"

#include "CAttributeComponent.h"

ACHealthPickup::ACHealthPickup()
{
	AmountToHeal = 25;
}

void ACHealthPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	if (! ensure(InstigatorPawn))
	{
		return;
	}

	UCAttributeComponent* AttributeComp = Cast<UCAttributeComponent>(InstigatorPawn->GetComponentByClass(UCAttributeComponent::StaticClass()));
	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{		
		if (AttributeComp->ApplyHealthChange(AmountToHeal)) // Try to apply the change. Go on cooldown if successful
		{
			StartCooldown();
		}		
	}
	

}



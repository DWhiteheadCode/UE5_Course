// Fill out your copyright notice in the Description page of Project Settings.


#include "CHealthPickup.h"

#include "CAttributeComponent.h"

ACHealthPickup::ACHealthPickup()
{
	AmountToHeal = 25;
}

void ACHealthPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	if (UCAttributeComponent* InstigatorAttributeComp = GetInstigatorAttributeComp(InstigatorPawn))
	{
		if (!bIsOnCooldown && !InstigatorIsMaxHealth(InstigatorAttributeComp))
		{
			InstigatorAttributeComp->ApplyHealthChange(AmountToHeal);

			StartCooldown();
		}
	}
}

bool ACHealthPickup::InstigatorIsMaxHealth(UCAttributeComponent* InstigatorAttributeComp)
{
	if (InstigatorAttributeComp)
	{
		if (InstigatorAttributeComp->GetHealth() == InstigatorAttributeComp->GetHealthMax())
		{
			return true;
		}
	}
	
	return false;
}

UCAttributeComponent* ACHealthPickup::GetInstigatorAttributeComp(APawn* InstigatorPawn)
{
	if (InstigatorPawn)
	{
		UCAttributeComponent* AttributeComp = Cast<UCAttributeComponent>(InstigatorPawn->GetComponentByClass(UCAttributeComponent::StaticClass()));

		if (AttributeComp)
		{
			return AttributeComp;
		}
	}

	return nullptr;
}
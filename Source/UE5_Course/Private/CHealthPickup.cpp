// Fill out your copyright notice in the Description page of Project Settings.


#include "CHealthPickup.h"

#include "CAttributeComponent.h"
#include "CPlayerState.h"

ACHealthPickup::ACHealthPickup()
{
	AmountToHeal = 25;
	CreditsCost = 250;
}

void ACHealthPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	if (! ensure(InstigatorPawn))
	{
		return;
	}

	ACPlayerState* PlayerState = Cast<ACPlayerState>(InstigatorPawn->GetPlayerState());
	if (! ensureMsgf(PlayerState, TEXT("Health potion interacted with by pawn without CPlayerState")))
	{
		return;
	}

	UCAttributeComponent* AttributeComp = UCAttributeComponent::GetAttributeComponent(InstigatorPawn);
	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{		
		if (PlayerState->SpendCredits(CreditsCost)
			&& AttributeComp->ApplyHealthChange(this, AmountToHeal)) 
		{
			StartCooldown();
		}		
	}
}

FText ACHealthPickup::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	UCAttributeComponent* AttributeComp = UCAttributeComponent::GetAttributeComponent(InstigatorPawn);
	if (AttributeComp && AttributeComp->IsFullHealth())
	{
		return NSLOCTEXT( "InteractableActors", "HealthPickup_FullHealthWarning", "Already at full health.");
	}

	return FText::Format(
		NSLOCTEXT("InteractableActors", "HealthPickup_InteractMessage", "Costs {0} Credits. Restores health to maximum."), 
		CreditsCost);
}



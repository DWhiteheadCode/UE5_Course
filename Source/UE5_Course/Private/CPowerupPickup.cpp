// Fill out your copyright notice in the Description page of Project Settings.


#include "CPowerupPickup.h"

#include "CActionComponent.h"

void ACPowerupPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn && ActionToGrant))
	{
		return;
	}

	UCActionComponent* ActionComp = Cast<UCActionComponent>(InstigatorPawn->GetComponentByClass(UCActionComponent::StaticClass()));

	if (ensure(ActionComp))
	{
		if (ActionComp->HasAction( ActionToGrant ))
		{
			FString DebugMsg = FString::Printf(TEXT("Action '%s' is already known."), *GetNameSafe(ActionToGrant));
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);

			return;
		}

		ActionComp->AddAction(ActionToGrant, this);
		StartCooldown();
	}
}
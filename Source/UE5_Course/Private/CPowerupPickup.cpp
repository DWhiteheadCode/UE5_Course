// Fill out your copyright notice in the Description page of Project Settings.


#include "CPowerupPickup.h"

#include "CActionComponent.h"

void ACPowerupPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	UCActionComponent* ActionComp = Cast<UCActionComponent>(InstigatorPawn->GetComponentByClass(UCActionComponent::StaticClass()));

	if (ensure(ActionComp))
	{
		ActionComp->AddAction(ActionToGrant, this);
		StartCooldown();
	}
}
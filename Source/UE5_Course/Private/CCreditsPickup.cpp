// Fill out your copyright notice in the Description page of Project Settings.


#include "CCreditsPickup.h"

#include "CPlayerState.h"

ACCreditsPickup::ACCreditsPickup()
{
	CreditsValue = 500;
}

void ACCreditsPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	ACPlayerState* PlayerState = Cast<ACPlayerState>(InstigatorPawn->GetPlayerState());
	if (!ensureMsgf(PlayerState, TEXT("Credits pickup interacted with by pawn without CPlayerState")))
	{
		return;
	}

	PlayerState->AddCredits(CreditsValue);
	StartCooldown();
}

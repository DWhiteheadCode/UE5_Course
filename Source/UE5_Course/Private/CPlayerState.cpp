// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerState.h"

ACPlayerState::ACPlayerState()
{
	Credits = 0;
}

int ACPlayerState::GetCredits() const
{
	return Credits;
}

void ACPlayerState::UpdateCredits(AActor* InstigatorActor, int Delta)
{
	int OldCredits = Credits;
	Credits = FMath::Max(0, Credits + Delta);
	int ActualDelta = Credits - OldCredits;

	OnCreditsChanged.Broadcast(InstigatorActor, this, Credits, ActualDelta);
}

bool ACPlayerState::SpendCredits(AActor* InstigatorActor, int Amount)
{
	if (Amount > Credits)
	{
		return false;
	}

	UpdateCredits( InstigatorActor, - Amount);

	return true;
}

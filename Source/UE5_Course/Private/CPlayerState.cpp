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

void ACPlayerState::AddCredits(int32 Delta)
{
	if ( ! ensure(Delta > 0.0f))
	{
		return;
	}

	Credits += Delta;

	OnCreditsChanged.Broadcast(this, Credits, Delta);
}

bool ACPlayerState::SpendCredits(int32 Amount)
{
	if ( ! ensure(Amount > 0.0f))
	{
		return false;
	}

	if (Amount > Credits)
	{
		return false;
	}

	Credits -= Amount;
	
	OnCreditsChanged.Broadcast(this, Credits, -Amount);

	return true;
}

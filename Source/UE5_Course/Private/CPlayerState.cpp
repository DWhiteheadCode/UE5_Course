// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerState.h"

#include "Net/UnrealNetwork.h"

ACPlayerState::ACPlayerState()
{
	Credits = 0;

	//bReplicates = true;
}

int ACPlayerState::GetCredits() const
{
	return Credits;
}

void ACPlayerState::AddCredits(int32 Delta)
{
	if (!ensureAlways(HasAuthority()))
	{
		return;
	}

	if ( ! ensure(Delta > 0.0f))
	{
		return;
	}

	Credits += Delta;

	MulticastCreditsChanged(Credits, Delta);
}

bool ACPlayerState::SpendCredits(int32 Amount)
{
	if (!ensureAlways(HasAuthority()))
	{
		return false;
	}

	if ( ! ensure(Amount > 0.0f))
	{
		return false;
	}

	if (Amount > Credits)
	{
		return false;
	}

	Credits -= Amount;
	
	MulticastCreditsChanged(Credits, -Amount);

	return true;
}

void ACPlayerState::MulticastCreditsChanged_Implementation(int NewCredits, int Delta)
{
	OnCreditsChanged.Broadcast(this, NewCredits, Delta);
}

void ACPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPlayerState, Credits);
}
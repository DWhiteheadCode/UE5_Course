// Fill out your copyright notice in the Description page of Project Settings.


#include "CActionEffect.h"

#include "CActionComponent.h"
#include "GameFramework/GameStateBase.h"


UCActionEffect::UCActionEffect()
{
	bAutoStart = true;
}

void UCActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.0f) // Some effects may have "infinite" duration
	{
		FTimerDelegate DurationDelegate;
		DurationDelegate.BindUFunction(this, "StopAction", Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Duration, DurationDelegate, Duration, false);
	}

	if (Period > 0.0f) // Some effects may have "infinite" period
	{
		FTimerDelegate PeriodDelegate;
		PeriodDelegate.BindUFunction(this, "ExecutePeriodicEffect", Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Period, PeriodDelegate, Period, true);
	}
}

void UCActionEffect::StopAction_Implementation(AActor* Instigator)
{
	// If the period is very close to running, run it anyway.
	if (GetWorld()->GetTimerManager().GetTimerRemaining( TimerHandle_Period ) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodicEffect(Instigator);
	}

	// Called after the above check as Super::StopAction removes tags etc which may impact the periodic effect
	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Duration);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Period);

	UCActionComponent* OwningComp = GetOwningComponent();
	if (OwningComp)
	{
		OwningComp->RemoveAction(this);
	}
}

float UCActionEffect::GetTimeRemaining() const
{
	if (AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>())
	{
		float EndTime = TimeStarted + Duration;
		return EndTime - GS->GetServerWorldTimeSeconds();
	}

	return Duration; // Default if game state hasn't replicated to this client yet
}

void UCActionEffect::ExecutePeriodicEffect_Implementation(AActor* Instigator)
{
	// Effect in derived classes
}


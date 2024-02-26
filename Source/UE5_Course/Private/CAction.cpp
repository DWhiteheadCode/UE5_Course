// Fill out your copyright notice in the Description page of Project Settings.


#include "CAction.h"

void UCAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Started Action: %s"), *GetNameSafe(this));
}

void UCAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopped Action: %s"), *GetNameSafe(this));
}

UWorld* UCAction::GetWorld() const
{
	// Outer is set when creating action via NewObject<>()
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if (Comp)
	{
		return Comp->GetWorld();
	}

	return nullptr;
}

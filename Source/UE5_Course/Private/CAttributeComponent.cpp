// Fill out your copyright notice in the Description page of Project Settings.


#include "CAttributeComponent.h"

#include "CGameModeBase.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("c.DamageMulitplier"), true, TEXT("Global damage multiplier for attribute components"), ECVF_Cheat);


// Sets default values for this component's properties
UCAttributeComponent::UCAttributeComponent()
{
	Health = 100;
	HealthMax = 100;
}


bool UCAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool UCAttributeComponent::IsFullHealth() const
{
	return (Health == HealthMax);
}


// Returns true if a change was applied and false otherwise.
// E.g.: If Delta < 0 && Health == 0, this will return false
bool UCAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if ( ! GetOwner()->CanBeDamaged() )
	{
		return false;
	}

	if (Delta < 0.0f) // Check that it was damage and not healing
	{
		Delta *= CVarDamageMultiplier.GetValueOnGameThread();
	}

	float OldHealth = Health;
	Health = FMath::Clamp( (Health + Delta), 0, HealthMax );
	float ActualDelta = Health - OldHealth;

	OnHealthChanged.Broadcast( InstigatorActor, this, Health, Delta ); // Note: Will broadcast even if ActualDelta == 0

	//Died
	if (ActualDelta < 0.0f && Health == 0.0f)
	{
		ACGameModeBase* GM = GetWorld()->GetAuthGameMode<ACGameModeBase>();
		if (GM)
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return ActualDelta != 0; 
}

float UCAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

float UCAttributeComponent::GetHealth() const
{
	return Health;
}

UCAttributeComponent* UCAttributeComponent::GetAttributeComponent(AActor* FromActor)
{
	if (FromActor)
	{
		return FromActor->FindComponentByClass<UCAttributeComponent>();
	}

	return nullptr;
}

bool UCAttributeComponent::IsActorAlive(AActor* Actor)
{
	if (UCAttributeComponent* AttributeComp = GetAttributeComponent(Actor))
	{
		return AttributeComp->IsAlive();
	}

	return false; // Default if Actor is null, or doesn't have AttributeComp
}

bool UCAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, - GetHealthMax());
}


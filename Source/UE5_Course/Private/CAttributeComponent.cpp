// Fill out your copyright notice in the Description page of Project Settings.


#include "CAttributeComponent.h"

#include "CGameModeBase.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("c.DamageMulitplier"), true, TEXT("Global damage multiplier for attribute components"), ECVF_Cheat);


// Sets default values for this component's properties
UCAttributeComponent::UCAttributeComponent()
{
	Health = 100;
	HealthMax = 100;

	Rage = 0;
	RageMax = 250;
	RageConversionAmount = 2.f;

	SetIsReplicatedByDefault(true);
}

// HEALTH ----------------------------------------------------------------------------

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
	if ( ! GetOwner()->CanBeDamaged() && Delta < 0.0f)
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

	OnHealthChanged.Broadcast( InstigatorActor, this, Health, ActualDelta ); // Note: Will broadcast even if ActualDelta == 0

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
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}


// RAGE ---------------------------------------------------------------------------
float UCAttributeComponent::GetRageMax() const
{
	return RageMax;
}

float UCAttributeComponent::GetRage() const
{
	return Rage;
}

bool UCAttributeComponent::IsFullRage() const
{
	return Rage == RageMax;
}

float UCAttributeComponent::GetRageFromDamage(float Damage) const
{
	return Damage *= RageConversionAmount;
}

bool UCAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	float OldRage = Rage;
	Rage = FMath::Clamp((Rage + Delta), 0, RageMax);
	float ActualDelta = Rage - OldRage;

	OnRageChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta); 

	return ActualDelta != 0;
}

bool UCAttributeComponent::SpendRage(AActor* InstigatorActor, float Amount)
{
	if (! ensure(Amount > 0.0f))
	{
		return false;
	}

	if (Amount > Rage)
	{
		return false;
	}

	ApplyRageChange(InstigatorActor, - Amount);

	return true;
}

// MISC ----------------------------------------------------------------------------
UCAttributeComponent* UCAttributeComponent::GetAttributeComponent(AActor* FromActor)
{
	if (FromActor)
	{
		return FromActor->FindComponentByClass<UCAttributeComponent>();
	}

	return nullptr;
}

void UCAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCAttributeComponent, Health);
	DOREPLIFETIME(UCAttributeComponent, HealthMax);
	DOREPLIFETIME(UCAttributeComponent, Rage);
	DOREPLIFETIME(UCAttributeComponent, RageMax);
	DOREPLIFETIME(UCAttributeComponent, RageConversionAmount);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "CAttributeComponent.h"


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


bool UCAttributeComponent::ApplyHealthChange(float Delta)
{
	Health = FMath::Clamp( Health + Delta, 0, HealthMax );

	OnHealthChanged.Broadcast( nullptr, this, Health, Delta );

	return true;
}

float UCAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

float UCAttributeComponent::GetHealth() const
{
	return Health;
}



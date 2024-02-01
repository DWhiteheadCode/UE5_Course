// Fill out your copyright notice in the Description page of Project Settings.


#include "CAttributeComponent.h"

// Sets default values for this component's properties
UCAttributeComponent::UCAttributeComponent()
{
	Health = 100;
}



bool UCAttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta;

	return true;
}




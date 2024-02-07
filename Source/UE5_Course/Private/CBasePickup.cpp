// Fill out your copyright notice in the Description page of Project Settings.


#include "CBasePickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ACBasePickup::ACBasePickup()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetSphereRadius(48.0f);
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetupAttachment(SphereComp);

	bIsOnCooldown = false;
	Cooldown = 10;
}

// Base implementation doesn't have any functionality aside from hiding the pickup for `Cooldown` 
void ACBasePickup::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!bIsOnCooldown)
	{
		StartCooldown();
	}
}

void ACBasePickup::StartCooldown()
{
	bIsOnCooldown = true;
	MeshComp->SetVisibility(false, true);
	GetWorldTimerManager().SetTimer(TimerHandle_PickupCooldown, this, &ACBasePickup::OnCooldownEnd, Cooldown);
}

void ACBasePickup::OnCooldownEnd()
{
	bIsOnCooldown = false;
	MeshComp->SetVisibility(true, true);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "CBasePickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ACBasePickup::ACBasePickup()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Pickup");
	SphereComp->SetSphereRadius(48.0f);

	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetupAttachment(SphereComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CooldownTime = 10;
}


void ACBasePickup::Interact_Implementation(APawn* InstigatorPawn)
{
	// Logic in child class(es)
}

void ACBasePickup::StartCooldown()
{
	MeshComp->SetVisibility(false, true);
	GetWorldTimerManager().SetTimer(TimerHandle_PickupCooldown, this, &ACBasePickup::OnCooldownEnd, CooldownTime);
	SetActorEnableCollision(false);
}

void ACBasePickup::OnCooldownEnd()
{	
	MeshComp->SetVisibility(true, true);
	SetActorEnableCollision(true);
}

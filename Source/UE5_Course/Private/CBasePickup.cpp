// Fill out your copyright notice in the Description page of Project Settings.


#include "CBasePickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

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

	bIsOnCooldown = false;

	bReplicates = true;
}


void ACBasePickup::Interact_Implementation(APawn* InstigatorPawn)
{
	// Logic in child class(es)
}

void ACBasePickup::StartCooldown()
{
	bIsOnCooldown = true;
	OnRep_StateChanged();
}

void ACBasePickup::OnCooldownEnd()
{	
	bIsOnCooldown = false;
	OnRep_StateChanged();
}

void ACBasePickup::OnRep_StateChanged()
{
	if (bIsOnCooldown)
	{
		MeshComp->SetVisibility(false, true);
		GetWorldTimerManager().SetTimer(TimerHandle_PickupCooldown, this, &ACBasePickup::OnCooldownEnd, CooldownTime);
		SetActorEnableCollision(false);
	}
	else
	{
		MeshComp->SetVisibility(true, true);
		SetActorEnableCollision(true);
	}
}

void ACBasePickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACBasePickup, bIsOnCooldown); 
}
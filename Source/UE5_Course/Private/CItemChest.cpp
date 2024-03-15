// Fill out your copyright notice in the Description page of Project Settings.


#include "CItemChest.h"

#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACItemChest::ACItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("Lid Mesh");
	LidMesh->SetupAttachment(BaseMesh);

	OpenPitch = 110.f;

	bReplicates = true;
	SetReplicateMovement(true);
	BaseMesh->SetIsReplicated(true);
}

void ACItemChest::OnActorLoaded_Implementation()
{
	UpdateLidPosition();
}

void ACItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpened = !bLidOpened;

	UpdateLidPosition();
}

void ACItemChest::OnRep_LidMoved()
{
	UpdateLidPosition();
}

void ACItemChest::UpdateLidPosition()
{
	float NewPitch = bLidOpened ? OpenPitch : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(NewPitch, 0, 0));
}

void ACItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACItemChest, bLidOpened);
}

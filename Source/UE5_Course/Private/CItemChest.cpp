// Fill out your copyright notice in the Description page of Project Settings.


#include "CItemChest.h"

#include "Components/StaticMeshComponent.h"

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
}


void ACItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	LidMesh->SetRelativeRotation( FRotator( OpenPitch, 0, 0 ) );
}

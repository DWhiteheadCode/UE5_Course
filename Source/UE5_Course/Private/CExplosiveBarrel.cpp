// Fill out your copyright notice in the Description page of Project Settings.


#include "CExplosiveBarrel.h"

#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ACExplosiveBarrel::ACExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	MeshComp->SetCollisionProfileName("PhysicsActor");
	MeshComp->SetSimulatePhysics(true);

	RootComponent = MeshComp;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("Radial Force");
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->ForceStrength = 2000.0f;
	RadialForceComp->Radius = 700.0f;
}

void ACExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MeshComp->OnComponentHit.AddDynamic(this, &ACExplosiveBarrel::OnHit);
}


void ACExplosiveBarrel::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void ACExplosiveBarrel::Explode()
{
	RadialForceComp->FireImpulse();
}


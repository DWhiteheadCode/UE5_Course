// Fill out your copyright notice in the Description page of Project Settings.


#include "CMagicProjectile.h"

// Sets default values
ACMagicProjectile::ACMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAICharacter.h"

// Sets default values
ACAICharacter::ACAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CBasePickup.h"
#include "CCreditsPickup.generated.h"

/**
 * 
 */
UCLASS()
class UE5_COURSE_API ACCreditsPickup : public ACBasePickup
{
	GENERATED_BODY()

public:
	ACCreditsPickup();

	void Interact_Implementation(APawn* InstigatorPawn) override;

protected: 
	UPROPERTY(EditAnywhere, Category = "Credits Pickup")
	int CreditsValue;


};

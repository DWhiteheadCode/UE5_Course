// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CBasePickup.h"
#include "CPowerupPickup.generated.h"

class UCAction;

/**
 * 
 */
UCLASS()
class UE5_COURSE_API ACPowerupPickup : public ACBasePickup
{
	GENERATED_BODY()

public:
	void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Powerup Pickup")
	TSubclassOf<UCAction> ActionToGrant;
};

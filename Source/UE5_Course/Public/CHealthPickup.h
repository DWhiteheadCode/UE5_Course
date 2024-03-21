// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CBasePickup.h"
#include "CHealthPickup.generated.h"

class UCAttributeComponent;

UCLASS()
class UE5_COURSE_API ACHealthPickup : public ACBasePickup
{
	GENERATED_BODY()
	
	ACHealthPickup();

	void Interact_Implementation(APawn* InstigatorPawn) override;

	FText GetInteractText_Implementation(APawn* InstigatorPawn) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Health Pickup")
	float AmountToHeal;

	UPROPERTY(EditAnywhere, Category = "Health Pickup")
	int CreditsCost;

};

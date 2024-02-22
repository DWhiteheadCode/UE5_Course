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

protected:
	UPROPERTY(EditAnywhere)
	float AmountToHeal;

	UPROPERTY(EditAnywhere)
	int CreditsCost;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CActionEffect.h"
#include "CActionEffect_Thorns.generated.h"

class UCAttributeComponent;

/**
 * 
 */
UCLASS()
class UE5_COURSE_API UCActionEffect_Thorns : public UCActionEffect
{
	GENERATED_BODY()
	
public:
	UCActionEffect_Thorns();

	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float DamageFraction;

	UFUNCTION()
	void OnOwnerHealthChanged(AActor* InstigatorActor, UCAttributeComponent* OwningComp, float NewHealth, float Delta);
};

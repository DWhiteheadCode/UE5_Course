// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CActionEffect.h"
#include "CThornsEffect.generated.h"

class UCAttributeComponent;

/**
 * 
 */
UCLASS()
class UE5_COURSE_API UCThornsEffect : public UCActionEffect
{
	GENERATED_BODY()
	
public:
	UCThornsEffect();

	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float DamageFraction;

	UFUNCTION()
	void OnOwnerHealthChanged(AActor* InstigatorActor, UCAttributeComponent* OwningComp, float NewHealth, float Delta);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "CGameplayInterface.h"

#include "CBasePickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS(ABSTRACT)
class UE5_COURSE_API ACBasePickup : public AActor, public ICGameplayInterface
{
	GENERATED_BODY()
	
	virtual void Interact_Implementation(APawn* InstigatorPawn);

public:	
	ACBasePickup();

protected:
	//COMPONENTS --------------------------------------------
	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComp;

	// COOLDOWN ---------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CooldownTime;

	FTimerHandle TimerHandle_PickupCooldown;

	UFUNCTION()
	void StartCooldown();

	UFUNCTION()
	void OnCooldownEnd();

};

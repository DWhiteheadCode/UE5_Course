// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CAnimInstance.generated.h"

class UCActionComponent;

/**
 * 
 */
UCLASS()
class UE5_COURSE_API UCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsStunned;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	UCActionComponent* ActionComp;

	// Similar to BeginPlay(), but for animations
	void NativeInitializeAnimation() override;

	// Similar to Tick(), but for animations
	void NativeUpdateAnimation(float DeltaSeconds) override;

};

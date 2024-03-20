// Fill out your copyright notice in the Description page of Project Settings.


#include "CAnimInstance.h"

#include "GameplayTagContainer.h"
#include "CActionComponent.h"

void UCAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* OwningActor = GetOwningActor();
	if (OwningActor)
	{
		ActionComp = Cast<UCActionComponent>(OwningActor->GetComponentByClass(UCActionComponent::StaticClass()));
	}
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Get the "Status.Stunned" gameplay tag
	static FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");

	if (ActionComp)
	{
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(StunnedTag);
	}
}

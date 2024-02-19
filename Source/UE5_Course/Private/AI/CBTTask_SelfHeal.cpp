// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CBTTask_SelfHeal.h"

#include "AIController.h"
#include "CAttributeComponent.h"


EBTNodeResult::Type UCBTTask_SelfHeal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (! ensure(MyController))
	{
		return EBTNodeResult::Failed;
	}

	APawn* MyPawn = MyController->GetPawn();
	if (! ensure(MyPawn))
	{
		return EBTNodeResult::Failed;
	}

	UCAttributeComponent* MyAttributeComp = UCAttributeComponent::GetAttributeComponent(MyPawn);
	if (! ensureMsgf(MyAttributeComp, TEXT("Self heal node called on pawn without UCAttributeComponent")))
	{
		return EBTNodeResult::Failed;
	}

	float Delta = MyAttributeComp->GetHealthMax() - MyAttributeComp->GetHealth();

	MyAttributeComp->ApplyHealthChange(MyPawn, Delta); // Don't *think* result of health change impacts result of this node

	return EBTNodeResult::Succeeded;
}

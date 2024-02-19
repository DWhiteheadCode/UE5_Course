// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CBTService_CheckLowHealth.h"

#include "AIController.h"
#include "CAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

void UCBTService_CheckLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* OwnerController = OwnerComp.GetAIOwner();
	if ( ! ensure(OwnerController))
	{
		return;
	}

	APawn* MyPawn = OwnerController->GetPawn();
	if ( ! ensure(MyPawn))
	{
		return;
	}

	UCAttributeComponent* MyAttributeComp = UCAttributeComponent::GetAttributeComponent(MyPawn);
	if ( ! ensureMsgf(MyAttributeComp, TEXT("Can't call CheckLowHealth node on Actor without UCAttributeComponent")))
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackboardComp))
	{
		return;
	}


	float LowHealthThreshold = MyAttributeComp->GetHealthMax() * LowHealthPercentThreshold;

	if (MyAttributeComp->GetHealth() < LowHealthThreshold)
	{
		BlackboardComp->SetValueAsBool(LowHealthKey.SelectedKeyName, true);
	}
	else
	{
		BlackboardComp->SetValueAsBool(LowHealthKey.SelectedKeyName, false);
	}
}



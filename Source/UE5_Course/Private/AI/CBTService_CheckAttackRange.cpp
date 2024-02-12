// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CBTService_CheckAttackRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void UCBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackBoardComp))
	{
		AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject("TargetActor"));

		if (TargetActor) // No "ensure" here as apparently this could be null. Should only be null if TargetActor set incorrectly/ not set?
		{
			AAIController* MyController = OwnerComp.GetAIOwner();

			if (ensure(MyController))
			{
				APawn* MyPawn = MyController->GetPawn();

				if (ensure(MyPawn)) // Shouldn't be running behaviour tree if pawn is null, so check for that here.
				{
					float Distance = FVector::Distance(TargetActor->GetActorLocation(), MyPawn->GetActorLocation());
					bool bWithinRange = Distance < 2000.f;

					bool bHasLOS = false;
					if (bWithinRange) // Don't perform the check if the character will already be moving to Target
					{
						bHasLOS = MyController->LineOfSightTo(TargetActor);
					}

					BlackBoardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, (bWithinRange && bHasLOS));
				}
			}
		}
	}

}

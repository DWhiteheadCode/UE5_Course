// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void ACAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("BehaviorTree is nullptr! Assign BehaviorTree in AI controller")))
	{
		RunBehaviorTree(BehaviorTree);
	}	
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAICharacter.h"

#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
ACAICharacter::ACAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
}

void ACAICharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    PawnSensingComp->OnSeePawn.AddDynamic(this, &ACAICharacter::OnPawnSeen);
}

void ACAICharacter::OnPawnSeen(APawn* Pawn)
{
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        UBlackboardComponent* BlackboardComp = AIC->GetBlackboardComponent();
        BlackboardComp->SetValueAsObject("TargetActor", Pawn);
        DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
    }
}







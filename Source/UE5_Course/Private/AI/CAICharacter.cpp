// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAICharacter.h"

#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CAttributeComponent.h"
#include "BrainComponent.h"

// Sets default values
ACAICharacter::ACAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
    AttributeComp = CreateDefaultSubobject<UCAttributeComponent>("AttributeComp");

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ACAICharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    PawnSensingComp->OnSeePawn.AddDynamic(this, &ACAICharacter::OnPawnSeen);
    AttributeComp->OnHealthChanged.AddDynamic(this, &ACAICharacter::OnHealthChanged);
}

void ACAICharacter::SetTargetActor(AActor* TargetActor)
{
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        // BlackboardComponent should never be deleted while AIC is alive, so no if/ ensure
        AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", TargetActor);
    }
}

void ACAICharacter::OnPawnSeen(APawn* Pawn)
{
    SetTargetActor(Pawn);
    DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}

void ACAICharacter::OnHealthChanged(AActor* InstigatorActor, UCAttributeComponent* OwningComp, float NewHealth, float Delta)
{
    if (Delta < 0.0f)
    {
        if (InstigatorActor != this)
        {
            SetTargetActor(InstigatorActor);
        }

        if (NewHealth <= 0.0f)
        {
            // Stop BehaviorTree
            AAIController* AIC = Cast<AAIController>(GetController());
            if (AIC)
            {
                AIC->GetBrainComponent()->StopLogic("Killed");
            }

            // Ragdoll
            GetMesh()->SetAllBodiesSimulatePhysics(true);
            GetMesh()->SetCollisionProfileName("Ragdoll");

            // Call Destroy() in 5 seconds
            SetLifeSpan(5.0f);
        }        
    }
}

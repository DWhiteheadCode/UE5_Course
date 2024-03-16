// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAICharacter.h"

#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CAttributeComponent.h"
#include "BrainComponent.h"
#include "CWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CActionComponent.h"

// Sets default values
ACAICharacter::ACAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
    AttributeComp = CreateDefaultSubobject<UCAttributeComponent>("AttributeComp");
    ActionComp = CreateDefaultSubobject<UCActionComponent>("ActionComp");

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    TimeOfLastHitParameter = "TimeOfLastHit";

    PlayerSpottedWidgetDuration = 1.5f;

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
    GetMesh()->SetGenerateOverlapEvents(true);
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

AActor* ACAICharacter::GetTargetActor() const
{
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
    }

    return nullptr;
}

void ACAICharacter::OnPawnSeen(APawn* Pawn)
{
    if (GetTargetActor() != Pawn)
    {
        SetTargetActor(Pawn);

        MulticastPawnSeen();
    }

    //DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}

void ACAICharacter::MulticastPawnSeen_Implementation()
{
    if (ensure(PlayerSpottedWidgetClass) && PlayerSpottedWidgetInstance == nullptr)
    {
        PlayerSpottedWidgetInstance = CreateWidget<UCWorldUserWidget>(GetWorld(), PlayerSpottedWidgetClass);
    }

    if (ensure(PlayerSpottedWidgetInstance))
    {
        PlayerSpottedWidgetInstance->AttachedActor = this;

        if (!PlayerSpottedWidgetInstance->IsInViewport())
        {
            PlayerSpottedWidgetInstance->AddToViewport();
        }

        GetWorldTimerManager().ClearTimer(TimerHandle_PlayerSpottedWidget);
        GetWorldTimerManager().SetTimer(TimerHandle_PlayerSpottedWidget, this, &ACAICharacter::RemovePlayerSpottedWidget, PlayerSpottedWidgetDuration, false);
    }
}

void ACAICharacter::RemovePlayerSpottedWidget()
{
    if (ensure(PlayerSpottedWidgetInstance))
    {
        PlayerSpottedWidgetInstance->RemoveFromParent();
    }
}



void ACAICharacter::OnHealthChanged(AActor* InstigatorActor, UCAttributeComponent* OwningComp, float NewHealth, float Delta)
{
    if (Delta < 0.0f)
    {
        if (InstigatorActor != this)
        {
            SetTargetActor(InstigatorActor);
        }        

        GetMesh()->SetScalarParameterValueOnMaterials(TimeOfLastHitParameter, GetWorld()->TimeSeconds);

        // Only create a healthbar if one doesn't exist for this minion
        if (ActiveHealthBarWidget == nullptr) 
        {
            ActiveHealthBarWidget = CreateWidget<UCWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
            if (ActiveHealthBarWidget)
            {
                ActiveHealthBarWidget->AttachedActor = this;
                ActiveHealthBarWidget->AddToViewport();
            }
        }        

        // Died
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

            // Disable capsule component's collision
            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            // Prevent gravity being applied as there's no longer collision
            GetCharacterMovement()->DisableMovement(); 

            // Call Destroy() in 5 seconds
            SetLifeSpan(5.0f);
        }
    }

    
}



// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CBTTask_RangedAttack.h"

#include "AiController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CAttributeComponent.h"

UCBTTask_RangedAttack::UCBTTask_RangedAttack()
{
	MaxWeaponSpread = 4.0f;
}

EBTNodeResult::Type UCBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());
		if (MyPawn == nullptr) 
		{
			return EBTNodeResult::Failed;
		}

		AActor* TargetActor = Cast<AActor>( OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		if (TargetActor == nullptr)
		{
			return EBTNodeResult::Failed;
		}
		
		if ( ! UCAttributeComponent::IsActorAlive(TargetActor))
		{
			return EBTNodeResult::Failed;
		}

		FVector MuzzleLocation = MyPawn->GetMesh()->GetSocketLocation("Muzzle_01");
		FVector DirectionToTarget = TargetActor->GetActorLocation() - MuzzleLocation;
		FRotator MuzzleRotation = DirectionToTarget.Rotation();

		MuzzleRotation.Pitch += FMath::RandRange( 0.f, MaxWeaponSpread );
		MuzzleRotation.Yaw += FMath::RandRange(-MaxWeaponSpread, MaxWeaponSpread);

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = MyPawn;

		AActor* NewProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);

		// Return Succeeded if the projectile successfully spawn, return Faield otherwise
		return NewProjectile ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}



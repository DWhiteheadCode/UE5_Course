// Fill out your copyright notice in the Description page of Project Settings.


#include "CAction_ProjectileAttack.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UCAction_ProjectileAttack::UCAction_ProjectileAttack()
{
	AttackAnimDelay = 0.2f;
	HandSocketName = "Muzzle_01"; // Default. Can be changed per `CAction_ProjectileAttack` via blueprints
}

void UCAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		Character->PlayAnimMontage(AttackAnim);

		UGameplayStatics::SpawnEmitterAttached(CastingVFX, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		FTimerHandle TimerHandle_AttackDelay;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
	}
}

void UCAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		// Start trace at the player's camera
		FRotator CameraRotation = InstigatorCharacter->GetControlRotation();
		FVector TraceStartLocation = InstigatorCharacter->GetPawnViewLocation();

		float TraceDistance = 2500.f;
		FVector TraceEndLocation = TraceStartLocation + (CameraRotation.Vector() * TraceDistance);

		// Use sphere of 25 radius for sweep
		FCollisionShape Shape;
		float TraceRadius = 5.f;
		Shape.SetSphere(TraceRadius);

		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		// Ignore player
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(InstigatorCharacter);

		FHitResult HitResult;
		// Returns true if there is a blocking hit
		if (GetWorld()->SweepSingleByObjectType(HitResult, TraceStartLocation, TraceEndLocation, FQuat::Identity, ObjParams, Shape, QueryParams))
		{
			TraceEndLocation = HitResult.ImpactPoint;
		}		

		FVector ProjectileSpawnLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);
		FRotator ProjectileRotation = UKismetMathLibrary::FindLookAtRotation(ProjectileSpawnLocation, TraceEndLocation);

		// Spawn the projectile at the character's hand, moving in the direction the camera is pointing
		FTransform SpawnTM = FTransform(ProjectileRotation, ProjectileSpawnLocation);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;

		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}

	StopAction(InstigatorCharacter);
}
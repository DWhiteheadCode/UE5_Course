// Fill out your copyright notice in the Description page of Project Settings.


#include "CInteractionComponent.h"

#include "CGameplayInterface.h"

void UCInteractionComponent::PrimaryInteract()
{
	FCollisionObjectQueryParams Params;
	Params.AddObjectTypesToQuery(ECC_WorldDynamic); // Check collisions with WorldDynamic objects

	AActor* MyOwner = GetOwner();
	ensure(MyOwner);

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	float TraceDistance = 1000.f;
	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	// Use a sphere for the collision sweep
	float Radius = 30.f;
	FCollisionShape Shape;
	Shape.SetSphere(Radius);

	TArray<FHitResult> Hits;
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, Params, Shape);
	
	// Set debug line colour based on whether or not the line trace was a hit
	// Red if it was a hit, blue otherwise
	FColor LineColor = bBlockingHit ? FColor::Red : FColor::Blue;

	for (FHitResult& HitResult : Hits)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, Radius, 32, LineColor, false, 2.0f);

		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			if (HitActor->Implements<UCGameplayInterface>())
			{
				APawn* OwnerPawn = Cast<APawn>(MyOwner);
				ICGameplayInterface::Execute_Interact(HitActor, OwnerPawn);
				break;
			}
		}		
	}
	

	DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 2.0f);
}

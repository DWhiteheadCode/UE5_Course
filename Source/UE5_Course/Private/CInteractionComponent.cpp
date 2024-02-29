// Fill out your copyright notice in the Description page of Project Settings.


#include "CInteractionComponent.h"

#include "CGameplayInterface.h"
#include "CWorldUserWidget.h"

static TAutoConsoleVariable<bool> CVarDrawDebugInteraction(TEXT("c.InteractionDebugDraw"), false, TEXT("Draw debug information for interaction component"), ECVF_Cheat);


UCInteractionComponent::UCInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceRadius = 30.0f;
	TraceDistance = 500.0f;
	TraceChannel = ECC_WorldDynamic;
}

void UCInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FindBestInteractable();
}

void UCInteractionComponent::FindBestInteractable()
{
	bool bDebugDraw = CVarDrawDebugInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams Params;
	Params.AddObjectTypesToQuery(TraceChannel); 

	AActor* MyOwner = GetOwner();
	if (ensure(MyOwner))
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

		// Use a sphere for the collision sweep
		FCollisionShape Shape;
		Shape.SetSphere(TraceRadius);

		TArray<FHitResult> Hits;
		bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, Params, Shape);

		// Set debug line colour based on whether or not the line trace was a hit
		// Red if it was a hit, blue otherwise
		FColor LineColor = bBlockingHit ? FColor::Red : FColor::Blue;

		FocusedActor = nullptr; // Clear reference before updating with trace result

		for (FHitResult& HitResult : Hits)
		{
			if (bDebugDraw)
			{
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, TraceRadius, 32, LineColor, false, 2.0f);
			}

			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				if (HitActor->Implements<UCGameplayInterface>())
				{
					FocusedActor = HitActor;
					break;
				}
			}
		}

		// Create widget, and/or update widget's AttachedActor
		if (FocusedActor)
		{
			if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
			{
				DefaultWidgetInstance = CreateWidget<UCWorldUserWidget>(GetWorld(), DefaultWidgetClass);
			}	

			if (DefaultWidgetInstance)
			{
				DefaultWidgetInstance->AttachedActor = FocusedActor;

				if ( ! DefaultWidgetInstance->IsInViewport())
				{
					DefaultWidgetInstance->AddToViewport();
				}
			}
		}
		else // Remove widget from screen
		{
			if (DefaultWidgetInstance)
			{
				DefaultWidgetInstance->RemoveFromParent();
			}
		}

		if (bDebugDraw)
		{
			DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 2.0f);
		}
	}	
}	

void UCInteractionComponent::PrimaryInteract()
{
	if (FocusedActor == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No focused actor to interact with.");
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	ICGameplayInterface::Execute_Interact(FocusedActor, OwnerPawn);
}



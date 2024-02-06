// Fill out your copyright notice in the Description page of Project Settings.


#include "CMagicProjectile.h"

#include "Components/SphereComponent.h"
#include "CAttributeComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


void ACMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ACMagicProjectile::OnActorOverlap);
}


void ACMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator()) // Make sure an actor was hit
	{	
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation(), GetActorRotation());

		// Get the AttributeComponent that belongs to OtherActor
		// GetComponentByClass returns a "UActorComponent*", which we then cast to "UCAttributeComponent"
		//     GetComponentByClass takes the class of the component we want. This is acquired using the ::StaticClass() method on our class
		UCAttributeComponent* AttributeComp = Cast<UCAttributeComponent>( OtherActor->GetComponentByClass( UCAttributeComponent::StaticClass() ) );

		if (AttributeComp) // OtherActor might not have an AttributeComponent, in which case this would be nullptr
		{
			AttributeComp->ApplyHealthChange(-20.0f);
			this->Destroy(); // The projectile has hit something, so it can be destroyed
		}
	}
}


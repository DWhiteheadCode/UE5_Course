// Fill out your copyright notice in the Description page of Project Settings.


#include "CTestDummy.h"

#include "Components/StaticMeshComponent.h"
#include "CAttributeComponent.h"

// Sets default values
ACTestDummy::ACTestDummy()
{
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
    RootComponent = MeshComp;

    AttributeComp = CreateDefaultSubobject<UCAttributeComponent>("AttributeComp");
    AttributeComp->OnHealthChanged.AddDynamic( this, &ACTestDummy::OnHealthChanged );
}

void ACTestDummy::OnHealthChanged(AActor* InstigatorActor, UCAttributeComponent* OwningComp, float NewHealth, float Delta)
{
    if (Delta < 0.f)
    {
        MeshComp->SetScalarParameterValueOnMaterials("TimeOfLastHit", GetWorld()->TimeSeconds);
    }    
}

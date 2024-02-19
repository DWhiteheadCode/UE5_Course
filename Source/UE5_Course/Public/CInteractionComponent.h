// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_COURSE_API UCInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	void PrimaryInteract();

};

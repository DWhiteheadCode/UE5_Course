// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "CGameplayInterface.h"

#include "CItemChest.generated.h"

class UStaticMeshComponent;

UCLASS()
class UE5_COURSE_API ACItemChest : public AActor, public ICGameplayInterface
{
	GENERATED_BODY()
	
	void Interact_Implementation(APawn* InstigatorPawn);

public:	

	UPROPERTY(EditAnywhere)
	float OpenPitch;

	// Sets default values for this actor's properties
	ACItemChest();

protected:
	UPROPERTY(ReplicatedUsing="OnRep_LidMoved")
	bool bLidOpened;

	UFUNCTION()
	void OnRep_LidMoved();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;

};

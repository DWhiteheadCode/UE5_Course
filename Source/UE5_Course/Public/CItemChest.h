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
	
public:	
	ACItemChest();

	void OnActorLoaded_Implementation() override;

	void Interact_Implementation(APawn* InstigatorPawn) override;

	UPROPERTY(EditAnywhere)
	float OpenPitch;

protected:
	UPROPERTY(ReplicatedUsing="OnRep_LidMoved", BlueprintReadOnly, SaveGame)
	bool bLidOpened;

	UFUNCTION()
	void OnRep_LidMoved();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;

	UFUNCTION()
	void UpdateLidPosition();

};

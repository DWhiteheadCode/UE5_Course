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
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LidMesh;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

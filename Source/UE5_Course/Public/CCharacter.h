// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "CCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCInteractionComponent;
class UAnimMontage;

UCLASS()
class UE5_COURSE_API ACCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACCharacter();

protected:
	// Camera
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;


	// IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* MovementMappingContext;


	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	void Move(const FInputActionValue& Value);

	// Jump
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;


	// Look
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	void Look(const FInputActionValue& Value);


	// Primary Projectile
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* PrimaryProjectileAction;

	UPROPERTY(EditAnywhere, Category = PrimaryAttack)
	TSubclassOf<AActor> ProjectileClass;

	void PrimaryAttack_Start();

	UPROPERTY(EditAnywhere, Category = PrimaryAttack)
	UAnimMontage* AttackAnim;

	UFUNCTION()
	void PrimaryAttack_FireProjectile();

	UFUNCTION()
	void GetPrimaryProjectileRotation( FRotator& out, const FVector& HandLocation );

	FTimerHandle TimerHandle_PrimaryAttack;


	// Interact Component
	UPROPERTY(VisibleAnywhere)
	UCInteractionComponent* InteractionComp;


	// Interact Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* PrimaryInteractAction;

	void PrimaryInteract();






	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

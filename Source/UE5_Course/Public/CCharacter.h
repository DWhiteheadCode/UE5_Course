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
class UCAttributeComponent;
class UCActionComponent;

UCLASS()
class UE5_COURSE_API ACCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACCharacter();

protected:
	// COMPONENTS ----------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCActionComponent* ActionComp;

	// HIT FLASH PARAMETER NAME ---------------------------------
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeOfLastHitParameterName;

	// INTERACT -------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PrimaryInteractAction;

	void PrimaryInteract();

	// EYE LOCATION ---------------------------------------------
	virtual FVector GetPawnViewLocation() const override;

	// IMC ------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* MovementMappingContext;


	// MOVEMENT -------------------------------------------------
	// Strafe movement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	void Move(const FInputActionValue& Value);

	// Jump
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	// Look
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	void Look(const FInputActionValue& Value);

	// Sprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	FName SprintActionName;

	void SprintStart();

	void SprintStop();

	// MAGIC PROJECTILE ---------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MagicProjectileAction;
	
	UFUNCTION()
	void MagicProjectileAttack_Start();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	FName MagicProjectileActionName;

	// BLACKHOLE PROJECTILE --------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BlackholeProjectileAction;

	UFUNCTION()
	void BlackholeAttack_Start();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	FName BlackholeProjectileActionName;
	
	// TELEPORT PROJECTILE ----------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* TeleportProjectileAction;

	UFUNCTION()
	void TeleportProjectile_Start();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	FName TeleportProjectileActionName;

	// PARRY ---------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ParryAction;

	UFUNCTION()
	void Parry_Start();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	FName ParryActionName;

	// ON HEALTH CHANGED ---------------------------------------------
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UCAttributeComponent* OwningComp, float NewHealth, float Delta);








	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Exec)
	void HealSelf(float Amount = 100);

};

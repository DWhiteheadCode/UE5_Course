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

	// Interact Component
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCInteractionComponent* InteractionComp;

	// Attribute Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCAttributeComponent* AttributeComp;

	// INTERACT -------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PrimaryInteractAction;

	void PrimaryInteract();

	// IMC ------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* MovementMappingContext;


	// MOVEMENT -------------------------------------------------
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

	// GENERIC PROJECTILE ----------------------------------------
	UFUNCTION()
	void GetProjectileSpawnRotation(FRotator& Out, const FVector& ProjectileSpawnLocation, float TraceDistance);


	// PRIMARY PROJECTILE ---------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PrimaryProjectileAction;

	UPROPERTY(EditAnywhere, Category = PrimaryAttack)
	TSubclassOf<AActor> PrimaryProjectileClass;
	
	UFUNCTION()
	void PrimaryAttack_Start();

	UPROPERTY(EditAnywhere, Category = PrimaryAttack)
	UAnimMontage* PrimaryAttackAnim;

	UFUNCTION()
	void PrimaryAttack_FireProjectile();

	FTimerHandle TimerHandle_PrimaryAttack;

	// BLACKHOLE PROJECTILE --------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BlackholeProjectileAction;

	UPROPERTY(EditAnywhere, Category = BlackholeAttack)
	TSubclassOf<AActor> BlackholeProjectileClass;

	UFUNCTION()
	void BlackholeAttack_Start();

	UPROPERTY(EditAnywhere, Category = BlackholeAttack)
	UAnimMontage* BlackholeAttackAnim;

	UFUNCTION()
	void BlackholeAttack_FireProjectile();

	FTimerHandle TimerHandle_BlackholeAttack;
	
	// TELEPORT PROJECTILE ----------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* TeleportProjectileAction;

	UPROPERTY(EditAnywhere, Category = BlackholeAttack)
	TSubclassOf<AActor> TeleportProjectileClass;

	UFUNCTION()
	void TeleportProjectile_Start();

	UPROPERTY(EditAnywhere, Category = BlackholeAttack)
	UAnimMontage* TeleportProjectileAnim;

	UFUNCTION()
	void TeleportProjectile_FireProjectile();

	FTimerHandle TimerHandle_TeleportProjectile;











	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

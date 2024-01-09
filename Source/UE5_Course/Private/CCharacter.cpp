// Fill out your copyright notice in the Description page of Project Settings.


#include "CCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"


// Sets default values
ACCharacter::ACCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	this->CameraComp = CreateDefaultSubobject<UCameraComponent>("Camera Component");

	this->SpringArmComp->SetupAttachment(RootComponent);
	this->CameraComp->SetupAttachment(SpringArmComp);
}



void ACCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MoveValue = Value.Get<FVector2D>();

	if (GetController())
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MoveValue.X);

		FVector Right = GetActorRightVector();
		AddMovementInput(Right, MoveValue.Y);
	}
}

void ACCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookValue = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(LookValue.X);
		AddControllerPitchInput(LookValue.Y);
	}	
}



// Called when the game starts or when spawned
void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if ( APlayerController* PlayerController = Cast<APlayerController>(GetController()) )
	{
		if ( UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()) )
		{
			Subsystem->AddMappingContext(MovementMappingContext, 0);
		}
	}
}

// Called every frame
void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACCharacter::Look);
	}
}


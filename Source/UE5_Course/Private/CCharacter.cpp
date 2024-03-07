// Fill out your copyright notice in the Description page of Project Settings.


#include "CCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CInteractionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CAttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CActionComponent.h"


// Sets default values
ACCharacter::ACCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	CameraComp = CreateDefaultSubobject<UCameraComponent>("Camera Component");

	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringArmComp);

	SpringArmComp->bUsePawnControlRotation = false;

	InteractionComp = CreateDefaultSubobject<UCInteractionComponent>("Interaction Component");
	AttributeComp = CreateDefaultSubobject<UCAttributeComponent>("Attribute Component");
	ActionComp = CreateDefaultSubobject<UCActionComponent>("Action Component");

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;	

	TimeOfLastHitParameterName = "TimeOfLastHit";

	MagicProjectileActionName = "MagicProjectileAttack";
	BlackholeProjectileActionName = "BlackholeProjectileAttack";
	TeleportProjectileActionName = "TeleportProjectileAttack";
	SprintActionName = "Sprint";
	ParryActionName = "Parry";
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MovementMappingContext, 0);
		}
	}
}

void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// -- Rotation Visualization -- //
	const float DrawScale = 100.0f;
	const float Thickness = 5.0f;

	FVector LineStart = GetActorLocation();
	// Offset to the right of pawn
	LineStart += GetActorRightVector() * 100.0f;
	// Set line end in direction of the actor's forward
	FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	// Draw Actor's Direction
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

	FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Blue, false, 0.0f, 0, Thickness);

}

void ACCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ACCharacter::OnHealthChanged);
}

// Called to bind functionality to input
void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACCharacter::Look);
		EnhancedInputComponent->BindAction(MagicProjectileAction, ETriggerEvent::Started, this, &ACCharacter::MagicProjectileAttack_Start);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACCharacter::Jump);
		EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ACCharacter::PrimaryInteract);
		EnhancedInputComponent->BindAction(BlackholeProjectileAction, ETriggerEvent::Started, this, &ACCharacter::BlackholeAttack_Start);
		EnhancedInputComponent->BindAction(TeleportProjectileAction, ETriggerEvent::Started, this, &ACCharacter::TeleportProjectile_Start);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACCharacter::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACCharacter::SprintStop);
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &ACCharacter::Parry_Start);
	}
}

FVector ACCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}


void ACCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MoveValue = Value.Get<FVector2D>();


	if (GetController())
	{
		// Get the Yaw (left/right) component from the camera
		FRotator ControlRotator = GetControlRotation();
		ControlRotator.Pitch = 0.f;
		ControlRotator.Roll = 0.f;

		FVector Forward = ControlRotator.Vector();
		AddMovementInput(Forward, MoveValue.X);

		FVector Right = FRotationMatrix(ControlRotator).GetScaledAxis(EAxis::Y); // Get the Y (right) vector from the ControlRotator
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

void ACCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, SprintActionName);
}

void ACCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, SprintActionName);
}

void ACCharacter::MagicProjectileAttack_Start()
{
	UE_LOG(LogTemp, Log, TEXT("Calling magic projectile action"));
	ActionComp->StartActionByName(this, MagicProjectileActionName);
}

void ACCharacter::BlackholeAttack_Start()
{
	ActionComp->StartActionByName(this, BlackholeProjectileActionName);
}

void ACCharacter::TeleportProjectile_Start()
{
	ActionComp->StartActionByName(this, TeleportProjectileActionName);
}

void ACCharacter::PrimaryInteract()
{
	if ( InteractionComp ) // Could avoid if-check for fail-fast? 
	{
		InteractionComp->PrimaryInteract();
	}
}


void ACCharacter::Parry_Start()
{
	ActionComp->StartActionByName(this, ParryActionName);
}

void ACCharacter::OnHealthChanged(AActor* InstigatorActor, UCAttributeComponent* OwningComp, float NewHealth, float ActualDelta)
{
	if (ActualDelta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeOfLastHitParameterName, GetWorld()->TimeSeconds);

		float RageAmount = OwningComp->GetRageFromDamage( - ActualDelta);
		OwningComp->ApplyRageChange(InstigatorActor, RageAmount);
	}

	// Character has taken damage that drops health to/below 0
	if (NewHealth <= 0.0f && ActualDelta < 0.0f)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			DisableInput(PC);
		}
	}
}

void ACCharacter::HealSelf(float Amount /*Default = 100*/ )
{
	AttributeComp->ApplyHealthChange(this, Amount);
}
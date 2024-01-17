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


// Sets default values
ACCharacter::ACCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	this->CameraComp = CreateDefaultSubobject<UCameraComponent>("Camera Component");

	this->SpringArmComp->SetupAttachment(RootComponent);
	this->CameraComp->SetupAttachment(SpringArmComp);

	this->SpringArmComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	this->InteractionComp = CreateDefaultSubobject<UCInteractionComponent>("Interaction Component");
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

void ACCharacter::Jump()
{
	if (CanJump())
	{
		Super::Jump();
	}
}

void ACCharacter::ShootPrimaryProjectile()
{
	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	// Spawn transformation matrix
	// Spawn the projectile at the character's hand, moving towards the camera's rotation
	FTransform SpawnTM = FTransform( GetControlRotation(), HandLocation );

	// Struct containing a number of spawn properties
	// Spawn the Actor, regardless of whether or not it is colliding with something else
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>( ProjectileClass, SpawnTM, SpawnParams );
}


void ACCharacter::PrimaryInteract()
{
	if ( InteractionComp ) // Could avoid if-check for fail-fast? 
	{
		InteractionComp->PrimaryInteract();
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

// Called to bind functionality to input
void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACCharacter::Look);
		EnhancedInputComponent->BindAction(PrimaryProjectileAction, ETriggerEvent::Started, this, &ACCharacter::ShootPrimaryProjectile);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACCharacter::Jump);
		EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ACCharacter::PrimaryInteract);
	}
}


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

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;	

	RightHandSocketName = "Muzzle_01";
	LeftHandSocketName = "Muzzle_02";

	TimeOfLastHitParameterName = "TimeOfLastHit";
}

void ACCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ACCharacter::OnHealthChanged);
}



void ACCharacter::OnHealthChanged(AActor* InstigatorActor, UCAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeOfLastHitParameterName, GetWorld()->TimeSeconds);
	}

	// Character has taken damage that drops health to/below 0
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		if ( APlayerController* PC = Cast<APlayerController>(GetController()) )
		{
			DisableInput(PC);
		}
	}
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


void ACCharacter::PrimaryAttack_Start()
{
	PlayAnimMontage(PrimaryAttackAnim);

	UGameplayStatics::SpawnEmitterAttached(PrimaryAttackCastHandVFX, GetMesh(), "Muzzle_01");

	float Delay = 0.15f;
	GetWorldTimerManager().SetTimer( TimerHandle_PrimaryAttack, this, &ACCharacter::PrimaryAttack_FireProjectile, Delay );
}

void ACCharacter::PrimaryAttack_FireProjectile()
{
	FVector HandLocation = GetMesh()->GetSocketLocation(RightHandSocketName);

	FRotator ProjectileRotation;
	float TraceDistance = 10'000.f;
	GetProjectileSpawnRotation(ProjectileRotation, HandLocation, TraceDistance);

	// Spawn transformation matrix
	// Spawn the projectile at the character's hand, moving towards the camera's rotation
	FTransform SpawnTM = FTransform(ProjectileRotation, HandLocation);

	// Struct containing a number of spawn properties
	// Spawn the Actor, regardless of whether or not it is colliding with something else
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<AActor>(PrimaryProjectileClass, SpawnTM, SpawnParams);
}


void ACCharacter::BlackholeAttack_Start()
{
	PlayAnimMontage(BlackholeAttackAnim);

	float Delay = 0.15f;
	GetWorldTimerManager().SetTimer(TimerHandle_BlackholeAttack, this, &ACCharacter::BlackholeAttack_FireProjectile, Delay);
}

void ACCharacter::BlackholeAttack_FireProjectile()
{
	FVector HandLocation = GetMesh()->GetSocketLocation(LeftHandSocketName);

	FRotator ProjectileRotation;
	float TraceDistance = 2500.f;
	GetProjectileSpawnRotation(ProjectileRotation, HandLocation, TraceDistance);

	// Spawn the projectile at the character's hand, moving towards the camera's rotation
	FTransform SpawnTM = FTransform(ProjectileRotation, HandLocation);

	// Spawn the Actor, regardless of whether or not it is colliding with something else
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	AActor* Blackhole = GetWorld()->SpawnActor<AActor>(BlackholeProjectileClass, SpawnTM, SpawnParams);

}




void ACCharacter::GetProjectileSpawnRotation(FRotator& out, const FVector& ProjectileSpawnLocation, float TraceDistance)
{
	// Set up line trace params
	FCollisionObjectQueryParams LineTraceParams;
	LineTraceParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	LineTraceParams.AddObjectTypesToQuery(ECC_WorldStatic);
	LineTraceParams.AddObjectTypesToQuery(ECC_Pawn);

	// Set up line trace end point
	const FRotator CameraRotation = CameraComp->GetComponentRotation();
	const FVector CameraLocation = CameraComp->GetComponentLocation();

	FVector TraceEndLocation = CameraLocation + ( CameraRotation.Vector() * TraceDistance );

	// Perform Line Trace
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType( HitResult, CameraLocation, TraceEndLocation, LineTraceParams );

	// Calculate default result (if no hit)
	out = UKismetMathLibrary::FindLookAtRotation( ProjectileSpawnLocation, TraceEndLocation );

	// Calculate result if there was a hit
	if ( HitResult.bBlockingHit )
	{
		out = UKismetMathLibrary::FindLookAtRotation(ProjectileSpawnLocation, HitResult.ImpactPoint);
	}
}


void ACCharacter::TeleportProjectile_Start()
{
	PlayAnimMontage(TeleportProjectileAnim);

	float Delay = 0.15f;
	GetWorldTimerManager().SetTimer(TimerHandle_TeleportProjectile, this, &ACCharacter::TeleportProjectile_FireProjectile, Delay);
}

void ACCharacter::TeleportProjectile_FireProjectile()
{
	FVector HandLocation = GetMesh()->GetSocketLocation(RightHandSocketName);

	FRotator ProjectileRotation;
	float TraceDistance = 1000.f;
	GetProjectileSpawnRotation(ProjectileRotation, HandLocation, TraceDistance);

	// Spawn the projectile at the character's hand, moving towards the camera's rotation
	FTransform SpawnTM = FTransform(ProjectileRotation, HandLocation);

	// Spawn the Actor, regardless of whether or not it is colliding with something else
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<AActor>(TeleportProjectileClass, SpawnTM, SpawnParams);
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
		EnhancedInputComponent->BindAction(PrimaryProjectileAction, ETriggerEvent::Started, this, &ACCharacter::PrimaryAttack_Start);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACCharacter::Jump);
		EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ACCharacter::PrimaryInteract);
		EnhancedInputComponent->BindAction(BlackholeProjectileAction, ETriggerEvent::Started, this, &ACCharacter::BlackholeAttack_Start);
		EnhancedInputComponent->BindAction(TeleportProjectileAction, ETriggerEvent::Started, this, &ACCharacter::TeleportProjectile_Start);
	}
}


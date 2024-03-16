// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void ACPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MenusMappingContext, 100);
	}
}

void ACPlayerController::BeginPlayingState()
{
	BlueprintBeginPlayingState();
}

void ACPlayerController::TogglePauseMenu()
{
	UE_LOG(LogTemp, Log, TEXT("TogglePauseMenu():"));

	if (!ensure(PauseMenuClass))
	{
		return;
	}

	// Close Menu
	if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		UE_LOG(LogTemp, Log, TEXT("\tClose Menu"));

		PauseMenuInstance->RemoveFromParent();
		PauseMenuInstance = nullptr;

		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
	else // Open Menu
	{
		UE_LOG(LogTemp, Log, TEXT("\tOpen Menu"));

		PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
		
		if (ensure(PauseMenuInstance))
		{
			PauseMenuInstance->AddToViewport(100);

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
		
	}
}

void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(PauseMenuAction, ETriggerEvent::Started, this, &ACPlayerController::TogglePauseMenu);
	}
}

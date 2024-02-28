// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CPlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnCreditsChanged,
	ACPlayerState*, OwningComp,
	int, NewCredits,
	int, Delta
);


/**
 * 
 */
UCLASS()
class UE5_COURSE_API ACPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ACPlayerState();

	UPROPERTY(BlueprintAssignable)
	FOnCreditsChanged OnCreditsChanged;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	int32 GetCredits() const;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void AddCredits(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool SpendCredits(int32 Amount);


protected:
	UPROPERTY(VisibleAnywhere, Category = "Credits")
	int32 Credits;

};

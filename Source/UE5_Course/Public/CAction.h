// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "GameplayTags.h"

#include "CAction.generated.h"

class UWorld;
class UCActionComponent;


USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bIsRunning;

	UPROPERTY()
	AActor* Instigator;
};


UCLASS(Blueprintable)
class UE5_COURSE_API UCAction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool IsRunning() const;

	UWorld* GetWorld() const override;

	bool IsSupportedForNetworking() const override
	{
		return true;
	}

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantsTags;

	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	UFUNCTION(BlueprintCallable, Category = "Action")
	UCActionComponent* GetOwningComponent() const;

	UPROPERTY(ReplicatedUsing="OnRep_RepData")
	FActionRepData RepData;

	UFUNCTION()
	void OnRep_RepData();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(Replicated)
	float TimeStarted;

};

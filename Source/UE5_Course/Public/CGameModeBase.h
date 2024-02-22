// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "CGameModeBase.generated.h"


class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;

UCLASS()
class UE5_COURSE_API ACGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ACGameModeBase();

	virtual void StartPlay() override;

	UFUNCTION(Exec)
	void KillAll();

	virtual void OnActorKilled(AActor* VictimActor, AActor* KillerActor);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* MaxNumMinionsCurve;

	FTimerHandle TimerHandle_BotSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float BotSpawnIntervalSeconds;

	UFUNCTION()
	void BotSpawnTimerElapsed();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionClass;

	UFUNCTION()
	void OnBotSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RespawnPlayerTimerElapsed(AController* Controller);

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	int CreditsPerKill;
};

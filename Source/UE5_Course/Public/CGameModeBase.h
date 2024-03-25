// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

#include "Engine/DataTable.h"

#include "CGameModeBase.generated.h"


class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class UCSaveGame;
class UDataTable;
class UCMinionData;

USTRUCT(BlueprintType)
struct FMinionInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Set default values
	FMinionInfoRow()
	{
		SpawnWeight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 50.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MinionId;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//TSubclassOf<AActor> MinionClass;

	// Relative weight that this minion is spawned whenever a new minion is spawned
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnWeight;

	// Points required by GameMode to spawn this minion
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	// Credits awarded to this minion's killer
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};

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

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

protected:
	// SAVE GAME --------------------------------------------
	UPROPERTY()
	UCSaveGame* CurrentSaveGame;

	FString SlotName;

	// AI SPAWNS --------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UDataTable* MinionTable;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* MaxNumMinionsCurve;

	FTimerHandle TimerHandle_BotSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float BotSpawnIntervalSeconds;

	UFUNCTION()
	void BotSpawnTimerElapsed();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	void OnMinionLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation);

	//UPROPERTY(EditDefaultsOnly, Category = "AI")
	//TSubclassOf<AActor> MinionClass;

	UFUNCTION()
	void OnBotSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	// KILLS/ DEATHS -----------------------------------------
	UFUNCTION()
	void RespawnPlayerTimerElapsed(AController* Controller);

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	int CreditsPerKill;

	// PICKUPS
	UFUNCTION(BlueprintCallable, Category = "Pickups")
	void SpawnPickups();

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	int MaxNumHealthPickups;

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	TSubclassOf<AActor> HealthPickupClass;

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	int MaxNumCreditsPickups;

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	TSubclassOf<AActor> CreditsPickupClass;

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	UEnvQuery* SpawnPickupQuery;

	UFUNCTION()
	void OnHealthPickupSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnCreditsPickupSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
};

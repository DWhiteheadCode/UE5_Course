// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameModeBase.h"

#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EngineUtils.h"

#include "AI/CAICharacter.h"
#include "CAttributeComponent.h"

ACGameModeBase::ACGameModeBase()
{
	BotSpawnIntervalSeconds = 2.0f;
}

void ACGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawn, this, &ACGameModeBase::BotSpawnTimerElapsed, BotSpawnIntervalSeconds, true);
}

void ACGameModeBase::BotSpawnTimerElapsed()
{
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
		
	// Could be nullptr if SpawnBotQuery wasn't set properly in BP
	if (ensure(QueryInstance)) 
	{
		// EQS Queries run asynchronously, so to use the result we need to bind to `GetOnQueryFinishedEvent()`
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ACGameModeBase::OnBotSpawnQueryFinished);
	}	
}

void ACGameModeBase::OnBotSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Bot EQS Failed!"));
		return;
	}


	int32 NumOfAliveBots = 0;
	for ( TActorIterator<ACAICharacter> It(GetWorld()); It; ++It )
	{
		ACAICharacter* Bot = *It;

		UCAttributeComponent* AttributeComp = Cast<UCAttributeComponent>(Bot->GetComponentByClass(UCAttributeComponent::StaticClass()));
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NumOfAliveBots++;
		}
	}

	float MaxBots = 10.0f; // Arbitrary default. Will be used if the ensureMsgf() fails
	if (ensureMsgf(MaxNumMinionsCurve, TEXT("Must set MaxNumMinionsCurve in UE5 GameMode BP")))
	{
		MaxBots = MaxNumMinionsCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (NumOfAliveBots >= MaxBots)
	{
		return;
	}


	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.Num() > 0)
	{		
		//FActorSpawnParameters SpawnParams;
		//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);
	}
}

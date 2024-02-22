// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameModeBase.h"

#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EngineUtils.h"

#include "AI/CAICharacter.h"
#include "CAttributeComponent.h"
#include "CCharacter.h"
#include "CPlayerState.h"

static TAutoConsoleVariable<bool> CVarSpawnBots( TEXT("c.SpawnBots"), false, TEXT("Enable bot spawning via timer"), ECVF_Cheat);

ACGameModeBase::ACGameModeBase()
{
	BotSpawnIntervalSeconds = 2.0f;
	CreditsPerKill = 100;
	MaxNumHealthPickups = 5;
	MaxNumCreditsPickups = 3;
}

void ACGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawn, this, &ACGameModeBase::BotSpawnTimerElapsed, BotSpawnIntervalSeconds, true);

	SpawnPickups();
}

void ACGameModeBase::BotSpawnTimerElapsed()
{
	if ( ! CVarSpawnBots.GetValueOnGameThread() )
	{
		UE_LOG(LogTemp, Log, TEXT("Not spawning bot due to 'c.SpawnBots' cvar"));
		return;
	}

	// Check the number of Alive bots
	int32 NumOfAliveBots = 0;
	for (TActorIterator<ACAICharacter> It(GetWorld()); It; ++It)
	{
		ACAICharacter* Bot = *It;

		UCAttributeComponent* AttributeComp = UCAttributeComponent::GetAttributeComponent(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NumOfAliveBots++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots"), NumOfAliveBots)

	float MaxBots = 10.0f; // Arbitrary default. Will be used if the ensureMsgf() fails
	if (ensureMsgf(MaxNumMinionsCurve, TEXT("Must set MaxNumMinionsCurve in UE5 GameMode BP")))
	{
		MaxBots = MaxNumMinionsCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	// If the number of Alive bots >= MaxBots, don't spawn a new bot
	if (NumOfAliveBots >= MaxBots)
	{
		UE_LOG(LogTemp, Log, TEXT("Max number of bots are alive. Skipping bot spawn."))
		return;
	}


	// Run EQS Query to check where the new bot should spawn
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
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.Num() > 0)
	{		
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	}	
}



void ACGameModeBase::KillAll() // @fixme: Healthbars sometimes remain on screen when this is called
{
	for (TActorIterator<ACAICharacter> It(GetWorld()); It; ++It)
	{
		ACAICharacter* Bot = *It;

		UCAttributeComponent* AttributeComp = UCAttributeComponent::GetAttributeComponent(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this);
		}
	}
}

void ACGameModeBase::OnActorKilled(AActor* VictimActor, AActor* KillerActor)
{
	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(KillerActor));

	// The killer was a player
	if (ACCharacter* KillerPlayer = Cast<ACCharacter>(KillerActor))
	{
		if (ACPlayerState* PlayerState = Cast<ACPlayerState>(KillerPlayer->GetPlayerState()))
		{
			// Using Killer as Instigator. Consider using gamemode/ victim? 
			PlayerState->UpdateCredits(KillerActor, CreditsPerKill);
		}
	}

	// The victim was a player
	if ( ACCharacter* VictimPlayer = Cast<ACCharacter>(VictimActor) )
	{		
		FTimerHandle TimerHandle_RespawnDelay; 
		FTimerDelegate Delegate;

		Delegate.BindUFunction(this, "RespawnPlayerTimerElapsed", VictimPlayer->GetController());

		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}
}

void ACGameModeBase::RespawnPlayerTimerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();
		RestartPlayer(Controller);
	}
}

void ACGameModeBase::SpawnPickups()
{
	// Spawn Health Pickups
	for (int i = 0; i < MaxNumHealthPickups; i++)
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnPickupQuery, this, EEnvQueryRunMode::RandomBest25Pct, nullptr);

		// Could be nullptr if SpawnBotQuery wasn't set properly in BP
		if (ensure(QueryInstance))
		{
			// EQS Queries run asynchronously, so to use the result we need to bind to `GetOnQueryFinishedEvent()`
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ACGameModeBase::OnHealthPickupSpawnQueryFinished);
		}
	}

	// Spawn Credits Pickups
	for (int i = 0; i < MaxNumCreditsPickups; i++)
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnPickupQuery, this, EEnvQueryRunMode::RandomBest25Pct, nullptr);

		// Could be nullptr if SpawnBotQuery wasn't set properly in BP
		if (ensure(QueryInstance))
		{
			// EQS Queries run asynchronously, so to use the result we need to bind to `GetOnQueryFinishedEvent()`
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ACGameModeBase::OnCreditsPickupSpawnQueryFinished);
		}
	}
}

void ACGameModeBase::OnHealthPickupSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Pickup EQS Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.Num() > 0)
	{
		GetWorld()->SpawnActor<AActor>(HealthPickupClass, Locations[0], FRotator::ZeroRotator);

		UE_LOG(LogTemp, Log, TEXT("Spawned HealthPickup"));
	}
}

void ACGameModeBase::OnCreditsPickupSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Credits EQS Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.Num() > 0)
	{
		 GetWorld()->SpawnActor<AActor>(CreditsPickupClass, Locations[0], FRotator::ZeroRotator);
	}
}

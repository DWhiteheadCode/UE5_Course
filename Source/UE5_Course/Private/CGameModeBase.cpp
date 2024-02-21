// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameModeBase.h"

#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EngineUtils.h"

#include "AI/CAICharacter.h"
#include "CAttributeComponent.h"
#include "CCharacter.h"

static TAutoConsoleVariable<bool> CVarSpawnBots( TEXT("c.SpawnBots"), true, TEXT("Enable bot spawning via timer"), ECVF_Cheat);

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
		//FActorSpawnParameters SpawnParams;
		//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
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
	if ( ACCharacter* Player = Cast<ACCharacter>(VictimActor) )
	{		
		FTimerHandle TimerHandle_RespawnDelay; 
		FTimerDelegate Delegate;

		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(KillerActor));
}

void ACGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();
		RestartPlayer(Controller);
	}
}

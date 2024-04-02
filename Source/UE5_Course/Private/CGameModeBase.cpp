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
#include "Kismet/GameplayStatics.h"
#include "CSaveGame.h"
#include "GameFramework/GameStateBase.h"
#include "CGameplayInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "CMinionData.h"
#include "CActionComponent.h"
#include "Engine/AssetManager.h"
#include "../UE5_Course.h"

static TAutoConsoleVariable<bool> CVarSpawnBots( TEXT("c.SpawnBots"), true, TEXT("Enable bot spawning via timer"), ECVF_Cheat);

ACGameModeBase::ACGameModeBase()
{
	BotSpawnIntervalSeconds = 2.0f;
	CreditsPerKill = 100;
	MaxNumHealthPickups = 5;
	MaxNumCreditsPickups = 3;

	PlayerStateClass = ACPlayerState::StaticClass();

	SlotName = "SaveGame01";
}

void ACGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	if (SelectedSaveSlot.Len() > 0)
	{
		SlotName = SelectedSaveSlot;
	}
}

void ACGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (ACPlayerState* PlayerState = NewPlayer->GetPlayerState<ACPlayerState>())
	{
		PlayerState->LoadPlayerState(CurrentSaveGame);
	}

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}


void ACGameModeBase::StartPlay()
{
	Super::StartPlay();

	LoadSaveGame();

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
		if (ensure(MinionTable))
		{
			TArray<FMinionInfoRow*> Rows;
			MinionTable->GetAllRows("", Rows);

			// Get random entry. Does NOT factor in SpawnWeight
			int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMinionInfoRow* SelectedRow = Rows[RandomIndex];


			UAssetManager* Manager = UAssetManager::GetIfValid();
			if (Manager)
			{
				TArray<FName> Bundles;
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ACGameModeBase::OnMinionLoaded, SelectedRow->MinionId, Locations[0]);
				
				Manager->LoadPrimaryAsset(SelectedRow->MinionId, Bundles, Delegate);
			}
		}		
	}	
}

void ACGameModeBase::OnMinionLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		UCMinionData* MinionData = Cast<UCMinionData>(Manager->GetPrimaryAssetObject(LoadedId));

		if (MinionData)
		{
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MinionData->MinionClass, SpawnLocation, FRotator::ZeroRotator);

			if (NewBot)
			{
				UCActionComponent* ActionComp = Cast<UCActionComponent>(NewBot->GetComponentByClass(UCActionComponent::StaticClass()));
				if (ActionComp)
				{
					for (TSubclassOf<UCAction> ActionClass : MinionData->Actions)
					{
						ActionComp->AddAction(ActionClass, NewBot);
					}
				}
				// SelectedRow specified actions to add, but NewBot doesn't have an ActionComp
				else if (!MinionData->Actions.IsEmpty())
				{
					UE_LOG(LogTemp, Warning, TEXT("Spawned bot [%s], which had no ActionComponent, but MinionData had [%i] actions to add. Actions were not added."),
						*GetNameSafe(NewBot),
						MinionData->Actions.Num())
				}
			}
		}
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

	// Award kill credit if the killer has PlayerState
	if (APawn* KillerPawn = Cast<APawn>(KillerActor))
	{
		if (ACPlayerState* PlayerState = KillerPawn->GetPlayerState<ACPlayerState>())
		{
			PlayerState->AddCredits(CreditsPerKill);
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

void ACGameModeBase::WriteSaveGame()
{   // Should add ensure() for CurrentSaveGame? 

	// Save Player States
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ACPlayerState* PlayerState = Cast<ACPlayerState>(GameState->PlayerArray[i]);
		if (PlayerState)
		{
			PlayerState->SavePlayerState(CurrentSaveGame);
			break; // Single player only
		}
	}

	// Save World Actors
	CurrentSaveGame->SavedActors.Empty(); // Clear value from previous load

	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if ( ! Actor->Implements<UCGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.Name = Actor->GetName();
		ActorData.Transform = Actor->GetTransform();


		FMemoryWriter MemWriter(ActorData.ByteData);
		FObjectAndNameAsStringProxyArchive Archive(MemWriter, true);
		Archive.ArIsSaveGame = true; // Only serialize variables marked with UPROPERTY(SaveGame)

		Actor->Serialize(Archive);


		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	
	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
	UE_LOG(LogTemp, Log, TEXT("Saved SaveGame"));
}

void ACGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<UCSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame data"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Successfully loaded SaveGame data"));

		for (FActorIterator It(GetWorld()); It; ++It)
		{			
			AActor* Actor = *It;
			if (!Actor->Implements<UCGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData LoadedActorData : CurrentSaveGame->SavedActors)
			{
				if (LoadedActorData.Name == Actor->GetName())
				{
					Actor->SetActorTransform( LoadedActorData.Transform );

					// Read data from ByteData into Archive
					FMemoryReader MemReader(LoadedActorData.ByteData);
					FObjectAndNameAsStringProxyArchive Archive(MemReader, true);
					Archive.ArIsSaveGame = true; 

					// Load data from the archive into this actor
					Actor->Serialize(Archive);

					ICGameplayInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}
		}
	}
	else
	{
		CurrentSaveGame = Cast<UCSaveGame>(UGameplayStatics::CreateSaveGameObject(UCSaveGame::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("Created new SaveGame data"));
	}
}



// Fill out your copyright notice in the Description page of Project Settings.


#include "CActionComponent.h"

#include "CAction.h"
#include "../UE5_Course.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UCActionComponent::UCActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UCActionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		for (TSubclassOf<UCAction> ActionClass : DefaultActions)
		{
			AddAction(ActionClass, GetOwner());
		}
	}	
}





// TODO: Disable component tick()
void UCActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//FString DebugMessage = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	//GEngine->AddOnScreenDebugMessage( -1, 0.0F, FColor::White, DebugMessage );

	//for (UCAction* Action : Actions)
	//{
		//FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;

		//FString ActionMsg = FString::Printf( TEXT("[%s] Action: %s"),
			//*GetNameSafe(GetOwner()),
			//*GetNameSafe(Action));

		//LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	//}

}

void UCActionComponent::AddAction(TSubclassOf<UCAction> ActionClass, AActor* Instigator)
{
	if (! ensure(ActionClass))
	{
		return;
	}

	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client attempted to AddAction to ActionComponent. [Class: %s]"), *GetNameSafe(ActionClass));
		return;
	}

	UCAction* NewAction = NewObject<UCAction>(this, ActionClass);
	if (ensure(NewAction))
	{
		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
}

bool UCActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (UCAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if ( ! Action->CanStart(Instigator))
			{
				FString FailedMsg = FString::Printf(TEXT("Couldn't start: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);

				continue; // Might have multiple actions with the same name?
			}

			// If this is running on a client
			if (!GetOwner()->HasAuthority())
			{
				// Tell the server to start the action in its version of this component
				ServerStartActionByName(Instigator, ActionName);
			}

			Action->StartAction(Instigator); // Start this action locally
			return true;
		}
	}

	return false;
}

void UCActionComponent::ServerStartActionByName_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

bool UCActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (UCAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning())
			{
				if (!GetOwner()->HasAuthority())
				{
					// Tell the server to stop the action in its version of this component
					ServerStopActionByName(Instigator, ActionName);
				}

				Action->StopAction(Instigator);
				return true;
			}			
		}
	}

	return false;
}

void UCActionComponent::ServerStopActionByName_Implementation(AActor* Instigator, FName ActionName)
{
	StopActionByName(Instigator, ActionName);
}

void UCActionComponent::RemoveAction(UCAction* ActionToRemove)
{
	if ( ! ensure(ActionToRemove && !ActionToRemove->IsRunning()) )
	{
		return;
	}

	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client attempted to RemoveAction to ActionComponent. [Action: %s]"), *GetNameSafe(ActionToRemove));
		return;
	}

	Actions.Remove(ActionToRemove);
}

bool UCActionComponent::HasAction(TSubclassOf<UCAction> ActionClass)
{
	for (UCAction* Action : Actions)
	{
		if (Action && Action->IsA(ActionClass))
		{
			return true;
		}
	}

	return false;
}

bool UCActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (UCAction* Action : Actions)
	{
		if (Action)
		{
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UCActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCActionComponent, Actions);
}
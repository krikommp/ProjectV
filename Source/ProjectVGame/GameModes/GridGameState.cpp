// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGameState.h"

#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "Containers/Array.h"
#include "Engine/EngineBaseTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameModes/GridExperienceManagerComponent.h"
#include "HAL/Platform.h"
#include "Misc/AssertionMacros.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridGameState)

class APlayerState;
class FLifetimeProperty;

extern ENGINE_API float GAverageFPS;

AGridGameState::AGridGameState(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UGridAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ExperienceManagerComponent = CreateDefaultSubobject<UGridExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));

	ServerFPS = 0.0f;
}

void AGridGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AGridGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/ this, /*Avatar=*/ this);
}

UAbilitySystemComponent* AGridGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGridGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGridGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void AGridGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
}

void AGridGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerFPS);
}

void AGridGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFPS = GAverageFPS;
	}
}

void AGridGameState::MulticastMessageToClients_Implementation(const FGridVerbMessage Message)
{
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}

void AGridGameState::MulticastReliableMessageToClients_Implementation(const FGridVerbMessage Message)
{
	MulticastMessageToClients_Implementation(Message);
}










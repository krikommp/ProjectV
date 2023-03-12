// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPlayerState.h"

#include "GridLogChannel.h"
#include "GridPlayerController.h"
#include "AbilitySystem/GridAbilitySet.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "Character/GridPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameModes/GridExperienceManagerComponent.h"
#include "GameModes/GridGameMode.h"
#include "Net/UnrealNetwork.h"

const FName AGridPlayerState::NAME_GridAbilityReady("GridAbilitiesReady");

AGridPlayerState::AGridPlayerState(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, MyPlayerConnectionType(EGridPlayerConnectionType::Player)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UGridAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AbilitySystemComponent 需要具有高优先级的 Update
	NetUpdateFrequency = 100.0f;
}

void AGridPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AGridPlayerState::Reset()
{
	Super::Reset();
}

void AGridPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (UGridPawnExtensionComponent* PawnExtComp = UGridPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void AGridPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	//@TODO: Copy stats
}

void AGridPlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;

	switch (GetPlayerConnectionType())
	{
	case EGridPlayerConnectionType::Player:
	case EGridPlayerConnectionType::InactivePlayer:
		//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
		// (e.g., for long running servers where they might build up if lots of players cycle through)
		bDestroyDeactivatedPlayerState = true;
		break;
	default:
		bDestroyDeactivatedPlayerState = true;
		break;
	}
	
	SetPlayerConnectionType(EGridPlayerConnectionType::InactivePlayer);

	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void AGridPlayerState::OnReactivated()
{
	if (GetPlayerConnectionType() == EGridPlayerConnectionType::InactivePlayer)
	{
		SetPlayerConnectionType(EGridPlayerConnectionType::Player);
	}
}

void AGridPlayerState::OnExperienceLoaded(const UGridExperienceDefinition* /*CurrentExperience*/)
{
	if (AGridGameMode* GridGameMode = GetWorld()->GetAuthGameMode<AGridGameMode>())
	{
		if (const UGridPawnData* NewPawnData = GridGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogGrid, Error, TEXT("AGridPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}

void AGridPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

AGridPlayerController* AGridPlayerState::GetGridPlayerController() const
{
	return Cast<AGridPlayerController>(GetOwner());
}

UAbilitySystemComponent* AGridPlayerState::GetAbilitySystemComponent() const
{
	return GetGridAbilitySystemComponent();
}

void AGridPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	if (GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UGridExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UGridExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnGridExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void AGridPlayerState::SetPawnData(const UGridPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogGrid, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	for (const UGridAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_GridAbilityReady);
	
	ForceNetUpdate();
}

void AGridPlayerState::OnRep_PawnData()
{
}

void AGridPlayerState::SetPlayerConnectionType(EGridPlayerConnectionType NewType)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);
	MyPlayerConnectionType = NewType;
}

void AGridPlayerState::ClientBroadcastMessage_Implementation(const FGridVerbMessage Message)
{
	// This check is needed to prevent running the action when in standalone mode
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}
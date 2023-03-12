// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessPieceExtensionComponent.h"

#include "GridChessPiece.h"
#include "GridChessPieceData.h"
#include "GridGameplayTags.h"
#include "GridLogChannel.h"
#include "AbilitySystem/GridAbilitySet.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Heros/GridHeroInfo.h"
#include "Net/UnrealNetwork.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(GridChessPieceExtensionComponent)

class FLifetimeProperty;
class UActorComponent;

const FName UGridChessPieceExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UGridChessPieceExtensionComponent::UGridChessPieceExtensionComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PieceData = nullptr;
	AbilitySystemComponent = nullptr;
}

void UGridChessPieceExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("GridPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UGridChessPieceExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one GridPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	RegisterInitStateFeature();
}

void UGridChessPieceExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// 监听所有 GameFeature 的变化
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	ensure(TryToChangeInitState(FGridGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UGridChessPieceExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UGridChessPieceExtensionComponent::SetChessPieceData(const UGridChessPieceData* InPieceData)
{
	check(InPieceData);

	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PieceData)
	{
		UE_LOG(LogGrid, Error, TEXT("Trying to set PieceData [%s] on pawn [%s] that already has valid PieceData [%s]."), *GetNameSafe(InPieceData), *GetNameSafe(Pawn), *GetNameSafe(PieceData));
		return;
	}

	PieceData = InPieceData;

	CheckDefaultInitialization();
}

bool UGridChessPieceExtensionComponent::IsFaction(const AGridChessPiece* Other) const
{
	if (const UGridChessPieceExtensionComponent* ExtensionComponent = UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(Other))
	{
		return HeroInfo->RuntimeFriendlyFactions.HasTag(ExtensionComponent->HeroInfo->RuntimeFaction);
	}
	return false;
}

void UGridChessPieceExtensionComponent::InitializeAbilitySystem(UGridAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		// 赋值一个新的 ASC 前，需要告知其他组件旧的 ASC 将被注销
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogGrid, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		UE_LOG(LogGrid, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		ensure(!ExistingAvatar->HasAuthority());

		if (UGridChessPieceExtensionComponent* OtherExtensionComponent = FindGridChessPieceExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	// if (ensure(PieceData))
	// {
	// 	InASC->SetTagRelationshipMapping(PieceData->TagRelationshipMapping);
	// }

	OnAbilitySystemInitialized.Broadcast();
}

void UGridChessPieceExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(FGridGameplayTags::Get().Ability_Behavior_SurvivesDeath);

		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		//AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}
}

void UGridChessPieceExtensionComponent::InitializeChessPieceInfo(UGridLocalPlayer* InLocalPlayer,
	UGridHeroInfo* InHeroInfo)
{
	check(InHeroInfo);

	Player = InLocalPlayer;
	HeroInfo = InHeroInfo;

	OnChessPieceInfoInitialized.Broadcast();
}

void UGridChessPieceExtensionComponent::InitializeAbilitySet()
{
	check(AbilitySystemComponent && PieceData);

	for (const UGridAbilitySet* AbilitySet : PieceData->AbilitySets)
	{
		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
	}
}

void UGridChessPieceExtensionComponent::OnChessPieceInitialized_RegisterAndCall(
	FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (HeroInfo != nullptr)
	{
		Delegate.Execute();
		return;
	}

	if (!OnChessPieceInfoInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnChessPieceInfoInitialized.Add(Delegate);
	}
}

void UGridChessPieceExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();

	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

bool UGridChessPieceExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();

	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!PieceData)
		{
			return false;
		}
		return true;
	}
	else if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataAvailable);
	}
	else if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UGridChessPieceExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (DesiredState == FGridGameplayTags::Get().InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
	}
}

void UGridChessPieceExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// If another feature is now in DataAvailable, see if we should transition to DataInitialized
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UGridChessPieceExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (AbilitySystemComponent)
	{
		Delegate.Execute();
		return;
	}
	
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}
}

void UGridChessPieceExtensionComponent::OnAbilitySystemUninitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}
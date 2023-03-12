#include "GridGameplayAbility_ChessPiece.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/AbilityEffects/GridAbilityTargetType.h"
#include "ChessPieces/GridChessPiece.h"
#include "ChessPieces/GridChessPieceExtensionComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GridMapManager/GridMapStateComponent.h"

UGridGameplayAbility_ChessPiece::UGridGameplayAbility_ChessPiece(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

UGridAbilitySystemComponent* UGridGameplayAbility_ChessPiece::GetGridAbilitySystemComponentFormActorInfo() const
{
	return (CurrentActorInfo ? Cast<UGridAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

AGridChessPiece* UGridGameplayAbility_ChessPiece::GetGridChessPieceFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AGridChessPiece>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

UGridChessPieceExtensionComponent* UGridGameplayAbility_ChessPiece::GetChessPieceExtensionComponent() const
{
	return (CurrentActorInfo ? UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

AGridMapManager* UGridGameplayAbility_ChessPiece::GetGridMapManager() const
{
	if (IsInstantiated())
	{
		if (const AGameStateBase* GameStateBase = GetWorld()->GetGameState())
		{
			if (const UGridMapStateComponent* MapStateComponent = GameStateBase->FindComponentByClass<UGridMapStateComponent>())
			{
				return MapStateComponent->GetGridMapManager();
			}
		}
	}
	return nullptr;
}

FGridGameplayEffectContainerSpec UGridGameplayAbility_ChessPiece::MakeEffectContainerSpecFromContainer(
	const FGridGameplayEffectContainer& Container, int32 TargetTileIndex, int32 Range,
	const FGameplayEventData& EventData)
{
	FGridGameplayEffectContainerSpec ContainerSpec;

	if (UGridAbilitySystemComponent* ASC = GetGridAbilitySystemComponentFormActorInfo())
	{
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			const UGridAbilityTargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
			AActor* AvatarActor = GetAvatarActorFromActorInfo();
			TargetTypeCDO->GetTargets(AvatarActor, GetWorld()->GetGameState(), TargetTileIndex, Range, EventData, HitResults, TargetActors);
			for (AActor* Actor : TargetActors)
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, EventData.EventTag, FGameplayEventData());
			}
			ContainerSpec.AddTargets(HitResults, TargetActors);
		}

		for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffects)
		{
			ContainerSpec.TargetGameplayEffectSpecHandles.Add(MakeOutgoingGameplayEffectSpec(EffectClass, GetAbilityLevel()));
		}
	}

	return ContainerSpec;
}

FGridGameplayEffectContainerSpec UGridGameplayAbility_ChessPiece::MakeEffectContainerSpec(FGameplayTag ContainerTag,
                                                                                          int32 TargetTileIndex, int32 Range, const FGameplayEventData& EventData)
{
	if (const FGridGameplayEffectContainer* FoundContainer = EffectContainers.Find(ContainerTag))
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer, TargetTileIndex, Range, EventData);
	}
	return FGridGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> UGridGameplayAbility_ChessPiece::ApplyEffectContainerSpec(
	const FGridGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> ActiveGameplayEffects;
	for(const auto& SpecHandle : ContainerSpec.TargetGameplayEffectSpecHandles)
	{
		ActiveGameplayEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return ActiveGameplayEffects;
}

TArray<FActiveGameplayEffectHandle> UGridGameplayAbility_ChessPiece::ApplyEffectContainer(FGameplayTag GameplayTag,
                                                                                    int32 TargetTileIndex, int32 Range, const FGameplayEventData& EventData)
{
	return ApplyEffectContainerSpec(MakeEffectContainerSpec(GameplayTag, TargetTileIndex, Range, EventData));
}

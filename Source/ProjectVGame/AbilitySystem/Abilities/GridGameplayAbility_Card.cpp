#include "GridGameplayAbility_Card.h"

#include "GridGameplayTags.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GridActionSet.h"
#include "ChessPieces/GridChessPiece.h"
#include "GridMapManager/GridMapFunctionLibrary.h"
#include "GridMapManager/GridMapManager.h"

UGridGameplayAbility_Card::UGridGameplayAbility_Card(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UGridGameplayAbility_Card::BeginDisplayCardAbilityRange(int32 StartIndex, int32 Range, int32 MinimumRange, bool bExcludeFriendly, bool bDisplay)
{
	if (AGridMapManager* GridMapManager = GetGridMapManager())
	{
		const float Z = ((GridMapManager->TileBoundsX + GridMapManager->TileBoundsY) * 0.5f) * 0.5f;
		GridMapManager->FindTilesInRange(StartIndex, Range, true, false, bDisplay, Z, MinimumRange, bExcludeFriendly);
	}
}

bool UGridGameplayAbility_Card::CheckMouseHoverTileInSlightRange(APlayerController* InController, int32& OutHoverIndex, FVector& OutHoverLocation)
{
	if (AGridMapManager* GridMapManager = GetGridMapManager())
	{
		if (GridMapManager->GetHitLocationAndIndex(InController, OutHoverIndex, OutHoverLocation))
		{
			if (GridMapManager->GetRangeArray().IsValidIndex(OutHoverIndex) && GridMapManager->GetRangeArray()[OutHoverIndex] != 0)
			{
				if (UGridMapFunctionLibrary::CompareClickLocation(GridMapManager, OutHoverLocation, OutHoverIndex))
				{
					return true;
				}
			}
		}
	}
	return false;
}

void UGridGameplayAbility_Card::EndDisplayCardAbilityRange()
{
	if (AGridMapManager* GridMapManager = GetGridMapManager())
	{
		GridMapManager->ClearAllInRangeMeshMarkers();
	}
}

const FGameplayTagContainer* UGridGameplayAbility_Card::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

bool UGridGameplayAbility_Card::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool OriginCostCheck = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	const UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	check(AbilitySystemComponent != nullptr);
	const float CardCost = GetCardAbilityCost(GetAbilityLevel());
	const UGridActionSet* ActionSet = AbilitySystemComponent->GetSet<UGridActionSet>();
	const bool CardCostCheck = ActionSet->GetActionPoint() >= CardCost;
	return OriginCostCheck && CardCostCheck;
}

void UGridGameplayAbility_Card::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGridGameplayTags GameplayTags = FGridGameplayTags::Get();
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(GameplayTags.Ability_Data_Cooldown, CooldownDuration.GetValueAtLevel(GetAbilityLevel()));
		// ReSharper disable once CppExpressionWithoutSideEffects
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

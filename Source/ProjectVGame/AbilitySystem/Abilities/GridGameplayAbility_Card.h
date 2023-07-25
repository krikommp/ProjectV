

#pragma once

#include "CoreMinimal.h"
#include "GridGameplayAbility_ChessPiece.h"

#include "GridGameplayAbility_Card.generated.h"

class UGridAbilitySystemComponent;
class AGridChessPiece;
class UGridChessPieceExtensionComponent;

UENUM(BlueprintType)
enum class ECardAbilityType : uint8
{
	// 需要指定目标
	NeedTarget,

	// 不需要指定目标
	NoTarget,

	UnKnown UMETA(Hidden),
};

/**
 * UGridGameplayAbility_Card
 *
 * 卡牌技能类
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by card."))
class PROJECTVGAME_API UGridGameplayAbility_Card : public UGridGameplayAbility_ChessPiece
{
	GENERATED_BODY()

public:

	UGridGameplayAbility_Card(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category="Grid|Card Ability")
	FORCEINLINE ECardAbilityType GetCardAbilityType() const { return CardAbilityType; }

	UFUNCTION(BlueprintCallable, Category="Grid|Card Ability")
	void BeginDisplayCardAbilityRange(int32 StartIndex, int32 Range, int32 MinimumRange, bool bExcludeFriendly, bool bDisplay);

	UFUNCTION(BlueprintCallable, Category="Grid|Card Ability")
	bool CheckMouseHoverTileInSlightRange(APlayerController* InController, int32& OutHoverIndex, FVector& OutHoverLocation);

	UFUNCTION(BlueprintCallable, Category="Grid|Card Ability")
	void EndDisplayCardAbilityRange();

	UFUNCTION(BlueprintCallable, Category="Grid|Card Ability")
	int32 GetCardAbilityCost(int32 InCardAbilityLevel) const { return FMath::Floor(CardAbilityCost.GetValueAtLevel(InCardAbilityLevel)); }

	UFUNCTION(BlueprintCallable, Category="Grid|Card Ability")
	int32 GetCardAbilityRange(int32 InCardAbilityLevel) const { return FMath::Floor(CardAbilityRange.GetValueAtLevel(InCardAbilityLevel)); }

	UFUNCTION(BlueprintCallable, Category="Grid|Card Ability")
	int32 GetCardAbilityTargetRange(int32 InCardAbilityLevel) const { return FMath::Floor(CardAbilityTargetRange.GetValueAtLevel(InCardAbilityLevel)); }

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Grid|Card Ability")
	FText GetCardAbilityCostForDisplay(int32 InCardAbilityLevel);

protected:

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:

	// 卡牌行为类型
	UPROPERTY(EditDefaultsOnly, Category="Grid|Card Behaviour")
	ECardAbilityType CardAbilityType;

	// 卡牌消耗
	UPROPERTY(EditDefaultsOnly, Category="Grid|Card Config")
	FScalableFloat CardAbilityCost;

	// 卡牌影响范围
	UPROPERTY(EditDefaultsOnly, Category="Grid|Card Config")
	FScalableFloat CardAbilityRange;

	// 卡牌查找目标范围
	UPROPERTY(EditDefaultsOnly, Category="Grid|Card Config")
	FScalableFloat CardAbilityTargetRange;

	// 卡牌冷却
	UPROPERTY(EditDefaultsOnly, Category="Grid|Card Config")
	FScalableFloat CooldownDuration;

	// 冷却GE
	UPROPERTY(EditDefaultsOnly, Category="Grid|Card Config")
	FGameplayTagContainer CooldownTags;

private:

	UPROPERTY()
	FGameplayTagContainer TempCooldownTags;
};



#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "UObject/Object.h"
#include "GridGameplayEffectContainer.generated.h"

class UGridAbilityTargetType;
class UGameplayEffect;

/**
 * FGridGameplayEffectContainer
 *
 * 保存 Gameplay Effect 的容器
 */
USTRUCT(BlueprintType)
struct PROJECTVGAME_API FGridGameplayEffectContainer
{
	GENERATED_BODY()

	FGridGameplayEffectContainer() {}

	// 查找应用 GameplayEffect 的目标
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GameplayEffect")
	TSubclassOf<UGridAbilityTargetType> TargetType;

	// 需要应用到目标上的 GameplayEffect 集合
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GameplayEffect")
	TArray<TSubclassOf<UGameplayEffect>> TargetGameplayEffects;
};

USTRUCT(BlueprintType)
struct PROJECTVGAME_API FGridGameplayEffectContainerSpec
{
	GENERATED_BODY()

	FGridGameplayEffectContainerSpec() {}

	UPROPERTY(BlueprintReadOnly, Category="GameplayEffect")
	FGameplayAbilityTargetDataHandle TargetData;

	UPROPERTY(BlueprintReadOnly, Category="GameplayEffect")
	TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecHandles;

	/** 检测是否有有效 GE */
	bool HasValidEffects() const;

	/** 检测是否有有效 Target */
	bool HasValidTargets() const;

	/** 添加新的目标对象 */
	void AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);
};
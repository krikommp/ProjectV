

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "UObject/Object.h"
#include "GridAbilityTargetType.generated.h"

/**
 * UGridAbilityTargetType
 *
 * 定义查找目标的方式
 */
UCLASS(BlueprintType, Blueprintable, meta=(ShowWorldContextPin))
class PROJECTVGAME_API UGridAbilityTargetType : public UObject
{
	GENERATED_BODY()

public:
	UGridAbilityTargetType() {}

	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(AActor* TargetingActor, AGameStateBase* GameState, int32 TargetTileIndex, int32 Range, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

UCLASS(NotBlueprintType)
class PROJECTVGAME_API UGridAbilityTargetType_User : public UGridAbilityTargetType
{
	GENERATED_BODY()
	
public:
	UGridAbilityTargetType_User() {}

	virtual void GetTargets_Implementation(AActor* TargetingActor, AGameStateBase* GameState, int32 TargetTileIndex, int32 Range, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

UCLASS(NotBlueprintable)
class PROJECTVGAME_API UGridAbilityTargetType_SingleOnGridMap : public UGridAbilityTargetType
{
	GENERATED_BODY()

public:
	UGridAbilityTargetType_SingleOnGridMap() {}

	virtual void GetTargets_Implementation(AActor* TargetingActor, AGameStateBase* GameState, int32 TargetTileIndex, int32 Range, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};



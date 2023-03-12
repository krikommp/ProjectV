

#pragma once

#include "CoreMinimal.h"
#include "GridAbilityEvent.h"
#include "UObject/Object.h"
#include "GridAbilityInputEvent.generated.h"

/**
 * UGridAbilityInputEvent
 *
 * 卡牌输入事件参数
 */
USTRUCT(BlueprintType)
struct PROJECTVGAME_API FGridAbilityInputEvent
{
	GENERATED_BODY()

	FGridAbilityInputEvent();

	UPROPERTY(BlueprintReadWrite, Category="Grid|Ability Event")
	FGameplayTag TriggerGameplayTag;

	UPROPERTY(BlueprintReadWrite, Category="Grid|Ability Event")
	int32 TargetTileIndex;
};

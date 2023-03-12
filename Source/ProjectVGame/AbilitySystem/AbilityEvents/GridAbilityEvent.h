

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "GridAbilityEvent.generated.h"

/**
 * UGridAbilityEvent
 *
 * 用于传递到 Gameplay Ability 中的事件参数
 */
USTRUCT(BlueprintType)
struct PROJECTVGAME_API FGridAbilityEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Grid|Ability Event")
	FGameplayTag TriggerGameplayTag;
};

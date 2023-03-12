

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GridGameplayAbility_Card.h"
#include "UObject/Object.h"
#include "GridCardBlueprintLibrary.generated.h"

class UGridGameplayAbility_Card;

/**
 * 
 */
UCLASS()
class PROJECTVGAME_API UGridCardBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category="Grid|Card Ability")
	static ECardAbilityType GetCardAbilityType(TSubclassOf<UGridGameplayAbility_Card> CardAbility);

	UFUNCTION(BlueprintPure, Category="Grid|Card Ability")
	static FText GetCardAbilityCostForText(TSubclassOf<UGridGameplayAbility_Card> CardAbility, int32 InCardAbilityLevel);
	
	UFUNCTION(BlueprintPure, Category="Grid|Card Ability")
	static int32 GetCardAbilityCost(TSubclassOf<UGridGameplayAbility_Card> CardAbility, int32 InCardAbilityLevel);

	UFUNCTION(BlueprintPure, Category="Grid|Card Ability")
	static int32 GetCardAbilityRange(TSubclassOf<UGridGameplayAbility_Card> CardAbility, int32 InCardAbilityLevel);
};

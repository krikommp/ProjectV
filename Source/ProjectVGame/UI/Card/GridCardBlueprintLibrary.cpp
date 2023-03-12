#include "GridCardBlueprintLibrary.h"

ECardAbilityType UGridCardBlueprintLibrary::GetCardAbilityType(TSubclassOf<UGridGameplayAbility_Card> CardAbility)
{
	return CardAbility->GetDefaultObject<UGridGameplayAbility_Card>()->GetCardAbilityType();
}

FText UGridCardBlueprintLibrary::GetCardAbilityCostForText(TSubclassOf<UGridGameplayAbility_Card> CardAbility, int32 InCardAbilityLevel)
{
	return CardAbility->GetDefaultObject<UGridGameplayAbility_Card>()->GetCardAbilityCostForDisplay(InCardAbilityLevel);
}

int32 UGridCardBlueprintLibrary::GetCardAbilityCost(TSubclassOf<UGridGameplayAbility_Card> CardAbility,
	int32 InCardAbilityLevel)
{
	return CardAbility->GetDefaultObject<UGridGameplayAbility_Card>()->GetCardAbilityCost(InCardAbilityLevel);
}

int32 UGridCardBlueprintLibrary::GetCardAbilityRange(TSubclassOf<UGridGameplayAbility_Card> CardAbility,
                                                     int32 InCardAbilityLevel)
{
	return CardAbility->GetDefaultObject<UGridGameplayAbility_Card>()->GetCardAbilityRange(InCardAbilityLevel);
}

#include "GridAbilityInputEvent.h"

#include "GridGameplayTags.h"

FGridAbilityInputEvent::FGridAbilityInputEvent()
{
	TriggerGameplayTag = FGridGameplayTags::Get().GameplayEvent_AbilityCardInput;
	TargetTileIndex = INDEX_NONE;
}

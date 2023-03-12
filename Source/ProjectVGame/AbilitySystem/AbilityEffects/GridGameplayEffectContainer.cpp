#include "GridGameplayEffectContainer.h"

bool FGridGameplayEffectContainerSpec::HasValidEffects() const
{
	return TargetGameplayEffectSpecHandles.Num() > 0;
}

bool FGridGameplayEffectContainerSpec::HasValidTargets() const
{
	return TargetData.Num() > 0;
}

void FGridGameplayEffectContainerSpec::AddTargets(const TArray<FHitResult>& HitResults,
	const TArray<AActor*>& TargetActors)
{
	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		TargetData.Add(NewData);
	}
}

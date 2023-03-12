// Fill out your copyright notice in the Description page of Project Settings.


#include "GridExperienceManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridExperienceManager)

#if WITH_EDITOR

void UGridExperienceManager::OnPlayInEditorBegun()
{
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}

void UGridExperienceManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		UGridExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UGridExperienceManager>();
		check(ExperienceManagerSubsystem);

		// 追踪被激活的 GameFeature
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
        ++Count;
	}
}

bool UGridExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	if (GIsEditor)
	{
		UGridExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UGridExperienceManager>();
		check(ExperienceManagerSubsystem);

		// Only let the last requester to get this far deactivate the plugin
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
		--Count;

		if (Count == 0)
		{
			ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);
			return true;
		}

		return false;
	}

	return true;
}



#endif
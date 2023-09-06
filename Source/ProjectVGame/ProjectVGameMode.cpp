
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "Settings/GridSettingDefaultValues.h"

#define LOCTEXT_NAMESPACE "FProjectVGameModule"

class FProjectVGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
#if WITH_EDITOR
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->RegisterSettings("Project", "Project", "GridSettingDefaultValues",
				LOCTEXT("GridSettingDefaultValuesName", "GridSettingDefaultValues"),
				LOCTEXT("GridSettingDefaultValuesDesc", "Setting for gameplay default values"),
				GetMutableDefault<UGridSettingDefaultValues>());
		}
#endif
	}

	virtual void ShutdownModule() override
	{
#if WITH_EDITOR
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->UnregisterSettings("Project", "Project", "GridSettingDefaultValues");
		}
#endif
	}
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_PRIMARY_GAME_MODULE(FProjectVGameModule, ProjectVGame, "ProjectVGame");
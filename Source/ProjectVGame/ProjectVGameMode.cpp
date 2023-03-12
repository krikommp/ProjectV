
#include "Modules/ModuleManager.h"

class FProjectVGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FProjectVGameModule, ProjectVGame, "ProjectVGame");
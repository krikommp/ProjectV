#include "ProjectVEditor.h"

#define LOCTEXT_NAMESPACE "ProjectVEditor"

DEFINE_LOG_CATEGORY(LogProjectVEditor);

class FProjectVEditorModule : public FDefaultGameModuleImpl {
	virtual void StartupModule() override {}

	virtual void ShutdownModule() override {}
	
};

IMPLEMENT_MODULE(FProjectVEditorModule, ProjectVEditor);

#undef LOCTEXT_NAMESPACE
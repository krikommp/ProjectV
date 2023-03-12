using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectVEditorTarget : TargetRules
{
	public ProjectVEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new string[] { "ProjectVGame", "ProjectVEditor" });
	}
}
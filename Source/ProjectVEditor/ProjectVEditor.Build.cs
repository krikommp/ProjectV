using UnrealBuildTool;

public class ProjectVEditor : ModuleRules
{
	public ProjectVEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"ProjectVEditor"
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"EditorFramework",
				"UnrealEd",
				"ProjectVGame"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"EditorStyle",
				"KismetWidgets"
			});
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
			}
		);
		
	}
}
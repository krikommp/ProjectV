
using UnrealBuildTool;

public class ProjectVGame : ModuleRules
{
	public ProjectVGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[]
			{
				"ProjectVGame"
			});
		
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
				"InputCore", 
				"HeadMountedDisplay", 
				"NavigationSystem", 
				"AIModule", 
				"Niagara",

				"GameplayTags",
				"GameplayTasks",
				"GameplayAbilities",
				
				"AIModule",
				
				"ModularGameplay",
				"ModularGameplayActors",
				
				"GameFeatures",
				
				"CommonLoadingScreen",
				
				"AsyncMixin",
				"ControlFlows",
			});
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore",
				"Slate",
				"SlateCore",
				"RenderCore",
				"DeveloperSettings",
				"EnhancedInput",
				"NetCore",
				"RHI",
				"Projects",
				"Gauntlet",
				"UMG",
				"CommonUI",
				"CommonInput",
				"GameSettings",
				"CommonGame",
				"CommonUser",
				"GameSubtitles",
				"GameplayMessageRuntime",
				"AudioMixer",
				"NetworkReplayStreaming",
				"UIExtension",
				"ClientPilot",
				"AudioModulation",
				"EngineSettings",
				"SMSystem"
			});

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
			}
		);

		// Generate compile errors if using DrawDebug functions in test/shipping builds.
		PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
	}
}
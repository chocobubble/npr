using UnrealBuildTool;

public class ToonForge : ModuleRules
{
	public ToonForge(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// Public include paths
			}
		);
				
		PrivateIncludePaths.AddRange(
			new string[] {
				// Private include paths
			}
		);
			
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"RenderCore",
				"Renderer",
				"RHI"
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorStyle",
				"EditorWidgets",
				"UnrealEd",
				"ToolMenus",
				"Slate",
				"SlateCore",
				"PropertyEditor",
				"EngineSettings",
				"Json",
				"JsonUtilities"
			}
		);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// Dynamically loaded modules
			}
		);
	}
}

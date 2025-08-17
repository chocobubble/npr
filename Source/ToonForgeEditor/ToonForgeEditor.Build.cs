using UnrealBuildTool;

public class ToonForgeEditor : ModuleRules
{
	public ToonForgeEditor(ReadOnlyTargetRules Target) : base(Target)
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
				"ToonForge"
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",
				"EditorStyle",
				"EditorWidgets",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"PropertyEditor",
				"DetailCustomizations",
				"Settings",
				"RenderCore",
				"Renderer",
				"RHI",
				"Projects",
				"InputCore",
				"LevelEditor",
				"WorkspaceMenuStructure"
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

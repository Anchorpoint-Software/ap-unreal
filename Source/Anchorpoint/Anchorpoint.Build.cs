// Some copyright should be here...

using System;
using System.IO;
using UnrealBuildTool;

public class Anchorpoint : ModuleRules
{
	public Anchorpoint(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"DeveloperSettings",
				"SourceControl",
				"AssetTools",
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"EditorSubsystem",
				"JsonUtilities",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Json", 
				"EditorScriptingUtilities",
				"ContentBrowserData",
				
				// Anchorpoint
				"AnchorpointCli", 
			});
	}
}

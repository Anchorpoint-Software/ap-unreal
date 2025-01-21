// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

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

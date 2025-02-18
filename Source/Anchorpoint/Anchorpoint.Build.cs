// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

using UnrealBuildTool;

public class Anchorpoint : ModuleRules
{
	public Anchorpoint(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AssetDefinition",
				"AssetTools",
				"ContentBrowser",
				"ContentBrowserData",
				"Core",
				"CoreUObject",
				"DeveloperSettings",
				"EditorFramework",
				"EditorScriptingUtilities",
				"EditorSubsystem",
				"Engine",
				"InputCore",
				"Json", 
				"JsonUtilities",
				"Projects",
				"Slate",
				"SlateCore",
				"SourceControl",
				"ToolMenus",
				"UnrealEd",

				// Anchorpoint
				"AnchorpointCli", 
			});
	}
}

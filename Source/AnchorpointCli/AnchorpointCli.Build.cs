// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

using UnrealBuildTool;

public class AnchorpointCli : ModuleRules
{
	public AnchorpointCli(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"EditorScriptingUtilities",
			"EditorSubsystem",
			"Engine",
			"Json",
			"JsonUtilities", 
			"Slate",
			"SlateCore",
			"SourceControl", 
			"UnrealEd",
			"ToolMenus",
			"UnsavedAssetsTracker",
		});
	}
}
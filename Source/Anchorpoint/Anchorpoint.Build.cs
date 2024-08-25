﻿// Some copyright should be here...

using System;
using System.IO;
using UnrealBuildTool;

public class Anchorpoint : ModuleRules
{
	public Anchorpoint(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
       
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
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
				
				// Anchorpoint
				"AnchorpointCli",
			});
	}
}

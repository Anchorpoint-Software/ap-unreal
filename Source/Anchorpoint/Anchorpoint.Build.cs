// Some copyright should be here...

using System;
using System.IO;
using UnrealBuildTool;

public class Anchorpoint : ModuleRules
{
	public Anchorpoint(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"EditorSubsystem",
			"Engine",
			"Slate",
			"SlateCore", 
			"SourceControl",
			"JsonUtilities"
		});

		PublicDefinitions.AddRange(
			new string[] {
				"OUTCOME_ENABLE_CXX_MODULES=0",
				"OUTCOME_FORCE_LEGACY_GCC_CXX_CONCEPTS=0"
			}
			);

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "apsync", "win", "sync.lib"));
			RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "apsync", "win", "sync.dll"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "apsync", "mac", "libsync.dylib"));
			RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "apsync", "mac", "libsync.dylib"));
		}

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "apsync", "include"));

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Json",
			}
			);
	}
}

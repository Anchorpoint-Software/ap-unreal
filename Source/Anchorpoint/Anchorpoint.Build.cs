// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class Anchorpoint : ModuleRules
{
	public Anchorpoint(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDefinitions.AddRange(
            new string[] {
                "OUTCOME_ENABLE_CXX_MODULES=0",
				"OUTCOME_FORCE_LEGACY_GCC_CXX_CONCEPTS=0"
            }
            );
        
		if (Target.Platform == UnrealTargetPlatform.Win64) {
			PublicAdditionalLibraries.AddRange(
				new string[] {
					// ... add public include paths required here ...
					Path.Combine(ModuleDirectory, "apsync", "win", "sync.lib")
				}
            );
			RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "apsync", "win", "sync.dll"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac) {
			PublicAdditionalLibraries.AddRange(
				new string[] {
					// ... add public include paths required here ...
					Path.Combine(ModuleDirectory, "apsync", "mac", "libsync.dylib")
				}
            );
			RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "apsync", "mac", "libsync.dylib"));
		}
        
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
                Path.Combine(ModuleDirectory, "apsync", "include")
			}
			);
				
		
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
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}

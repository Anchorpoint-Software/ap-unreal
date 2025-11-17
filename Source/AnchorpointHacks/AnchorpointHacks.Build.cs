// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

using System.IO;
using UnrealBuildTool;

public class AnchorpointHacks : ModuleRules
{
	public AnchorpointHacks(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"PackagesDialog",
				"Slate",
				"SlateCore",
			}
		);

		// Hacks to reach "Editor/PackagesDialog/Private/SPackagesDialog.h"
		PrivateIncludePaths.Add(Path.Combine(EngineDirectory, "Source/Editor/PackagesDialog/Private"));
	}
}

// Some copyright should be here...

using System.IO;
using UnrealBuildTool;

public class AnchorpointSync : ModuleRules
{
	public AnchorpointSync(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
		});

		PublicDefinitions.AddRange(new[]
		{
			"OUTCOME_ENABLE_CXX_MODULES=0",
			"OUTCOME_FORCE_LEGACY_GCC_CXX_CONCEPTS=0"
		});

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
	}
}
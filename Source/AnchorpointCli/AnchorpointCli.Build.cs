// Some copyright should be here...

using UnrealBuildTool;

public class AnchorpointCli : ModuleRules
{
	public AnchorpointCli(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"EditorSubsystem",
			"UnrealEd",
			"Json",
			"JsonUtilities",
		});
	}
}
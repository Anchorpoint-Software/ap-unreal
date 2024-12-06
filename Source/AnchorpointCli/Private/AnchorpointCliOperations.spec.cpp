#include <Misc/AutomationTest.h>

#include "AnchorpointCliUtils.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(AnchorpointCliOperationsTests, "Anchorpoint.CliOperations", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool AnchorpointCliOperationsTests::RunTest(const FString& Parameters)
{
	auto CompareCommands = [this](const FString& InCommand)
	{
		FCliParameters Parameters = {FString::Printf(TEXT("--print-config %s"), *InCommand)};
		Parameters.bRequestJsonOutput = true;
		Parameters.bUseIniFile = false;

		const FString& ExpectedIniFile = AnchorpointCliUtils::RunApCommand(Parameters).Output;
		const FString& ActualIniFile = AnchorpointCliUtils::ConvertCommandToIni(InCommand, true);

		const FString TestName = FString::Printf(TEXT("Comparing command %s"), *InCommand);
		TestEqual(TestName, ActualIniFile, ExpectedIniFile);
	};

	// Generic tests
	CompareCommands(TEXT("status"));
	CompareCommands(TEXT("git --command status"));
	CompareCommands(TEXT("git --command \"rm -- 'test1' 'test2' 'test3'\""));
	CompareCommands(TEXT("sync --message \"my commit message\""));
	CompareCommands(TEXT("user list"));

	// Parameter order shouldn't matter - they should match the order defined by the CLI (which hopefully doesn't matter)
	CompareCommands(TEXT("lock create --git --keep --files \"test1\" \"test2\" \"test3\""));
	CompareCommands(TEXT("lock create --git --files \"test1\" \"test2\" \"test3\" --keep"));
	CompareCommands(TEXT("lock create --files \"test1\" \"test2\" \"test3\" --git --keep"));
	CompareCommands(TEXT("lock create --files \"test1\" \"test2\" \"test3\" --keep --git"));
	CompareCommands(TEXT("lock create --keep --files \"test1\" \"test2\" \"test3\" --git"));
	CompareCommands(TEXT("lock create --keep --git --files \"test1\" \"test2\" \"test3\""));

	// Test with multiple file inputs
	CompareCommands(TEXT("lock remove --files \"test1\" \"test2\" \"test3\""));
	CompareCommands(TEXT("revert --files \"test1\" \"test2\" \"test3\""));

	return true;
}
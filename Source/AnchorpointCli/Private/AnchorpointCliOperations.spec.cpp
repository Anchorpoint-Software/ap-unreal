#include <Misc/AutomationTest.h>

#include "AnchorpointCliOperations.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(AnchorpointCliOperationsTests, "Anchorpoint.CliOperations", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool AnchorpointCliOperationsTests::RunTest(const FString& Parameters)
{
	auto CompareCommands = [this](const FString& InCommand)
	{
		FString CliCommand = FString::Printf(TEXT("--print-config %s"), *InCommand);
		const FString& ExpectedIniFile = AnchorpointCliOperations::RunApCommand(CliCommand).Output;
		const FString& ActualIniFile = AnchorpointCliOperations::ConvertCommandToIni(InCommand, true);

		const FString TestName = FString::Printf(TEXT("Comparing command %s"), *InCommand);
		TestEqual(TestName, ActualIniFile, ExpectedIniFile);
	};

	CompareCommands(TEXT("status"));
	CompareCommands(TEXT("git --command status"));
	CompareCommands(TEXT("git --command \"rm -- 'test' 'test' 'test'\""));

	return true;
}
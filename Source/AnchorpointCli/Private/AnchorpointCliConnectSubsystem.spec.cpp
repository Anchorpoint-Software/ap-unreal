// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include <Misc/AutomationTest.h>

#include "AnchorpointCliConnectSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(AnchorpointCliConnectSubsystemTests, "Anchorpoint.CliConnect", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool AnchorpointCliConnectSubsystemTests::RunTest(const FString& InParameters)
{
	const FString SingleRawMessage = TEXT(R"({"id":"2a267501-00e2-4d3d-bf20-484669794ddc","type":"project dirty"})");
	TOptional<TArray<FAnchorpointConnectMessage>> SingleParsedMessage = FAnchorpointConnectMessage::ParseStringToMessages(SingleRawMessage);

	TestTrue(TEXT("Passing message is parsed correctly"), SingleParsedMessage.IsSet());
	TestEqual(TEXT("Parsed message count"), SingleParsedMessage.GetValue().Num(), 1);
	TestEqual(TEXT("Parsed message ID"), SingleParsedMessage.GetValue()[0].Id, TEXT("2a267501-00e2-4d3d-bf20-484669794ddc"));
	TestEqual(TEXT("Parsed message type"), SingleParsedMessage.GetValue()[0].Type, TEXT("project dirty"));

	const FString DoubleRawMessage = TEXT(R"({"files":["Content/__ExternalActors__/ThirdPerson/Maps/ThirdPersonMap/1/29/Y97TNNVVOX822SLKQSOH6C.uasset"],"id":"35ce170a-65ba-4f7a-974e-0691160311b6","type":"files locked"}
											{"files":["Content/EF_Lewis/Materials/banner_Mat.uasset"],"id":"42e1245c-0fe9-413e-a132-29eef7fb6856","type":"files locked"})");

	TOptional<TArray<FAnchorpointConnectMessage>> DoubleParsedMessage = FAnchorpointConnectMessage::ParseStringToMessages(DoubleRawMessage);
	TestTrue(TEXT("Passing message is parsed correctly"), DoubleParsedMessage.IsSet());
	TestEqual(TEXT("Parsed message count"), DoubleParsedMessage.GetValue().Num(), 2);
	TestEqual(TEXT("Parsed message ID"), DoubleParsedMessage.GetValue()[0].Id, TEXT("35ce170a-65ba-4f7a-974e-0691160311b6"));
	TestEqual(TEXT("Parsed message type"), DoubleParsedMessage.GetValue()[0].Type, TEXT("files locked"));
	TestEqual(TEXT("Parsed message ID"), DoubleParsedMessage.GetValue()[1].Id, TEXT("42e1245c-0fe9-413e-a132-29eef7fb6856"));
	TestEqual(TEXT("Parsed message type"), DoubleParsedMessage.GetValue()[1].Type, TEXT("files locked"));

	const FString HalfRawMessage = TEXT(R"({"files":["Content/EF_Lewis/Materials/banner_Mat.uasset","Content/__Externa)");
	TOptional<TArray<FAnchorpointConnectMessage>> HalfParsedMessage = FAnchorpointConnectMessage::ParseStringToMessages(HalfRawMessage);
	TestFalse(TEXT("Passing message is NOT parsed"), HalfParsedMessage.IsSet());

	return true;
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "AnchorpointCliConnectSubsystem.h"

#include <ISourceControlModule.h>
#include <ISourceControlProvider.h>
#include <JsonObjectConverter.h>
#include <SourceControlOperations.h>
#include <Editor/EditorPerformanceSettings.h>
#include <Misc/InteractiveProcess.h>

#include "AnchorpointCli.h"

FString GetCliPath()
{
	const FString CliDirectory = FAnchorpointCliModule::Get().GetCliPath();

#if PLATFORM_WINDOWS
	return CliDirectory / "ap.exe";
#elif PLATFORM_MAC
	return CliDirectory / "ap";
#endif
}

DEFINE_LOG_CATEGORY_STATIC(LogAnchorpointCliConnect, Log, All);

void UAnchorpointCliConnectSubsystem::RespondToMessage(const FString& Id, TOptional<FString> Error)
{
	const FString ErrorString = Error.Get(TEXT(""));
	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Responding to message %s with error %s"), *Id, *ErrorString);

	FAnchorpointCliConnectResponse Response;
	Response.Id = Id;
	Response.Error = ErrorString;

	FString JsonMessage;
	FJsonObjectConverter::UStructToJsonObjectString<FAnchorpointCliConnectResponse>(Response, JsonMessage, 0, 0, 0, nullptr, false);

	Process->SendWhenReady(JsonMessage);
}

void UAnchorpointCliConnectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UEditorPerformanceSettings* EditorPerformanceSettings = GetMutableDefault<UEditorPerformanceSettings>();
	if (EditorPerformanceSettings)
	{
		EditorPerformanceSettings->bThrottleCPUWhenNotForeground = false;
		EditorPerformanceSettings->PostEditChange();
	}

	const FString CommandLineExecutable = GetCliPath();

	TArray<FString> Args;
	Args.Add(FString::Printf(TEXT("--cwd=\"%s\""), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));
	Args.Add(TEXT("connect --name \"unreal\""));
	const FString CommandLineArgs = FString::Join(Args, TEXT(" "));

	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Running %s %s"), *CommandLineExecutable, *CommandLineArgs);
	Process = MakeShared<FInteractiveProcess>(CommandLineExecutable, CommandLineArgs, true, true);

	if (!Process)
	{
		UE_LOG(LogAnchorpointCliConnect, Error, TEXT("Failed to create process"));
		return;
	}

	const bool bLaunchSuccess = Process->Launch();
	if (!bLaunchSuccess)
	{
		UE_LOG(LogAnchorpointCliConnect, Error, TEXT("Failed to launch process"));
		return;
	}

	Process->OnOutput().BindUObject(this, &UAnchorpointCliConnectSubsystem::OnOutput);
	Process->OnCompleted().BindUObject(this, &UAnchorpointCliConnectSubsystem::OnCompleted);
	Process->OnCanceled().BindUObject(this, &UAnchorpointCliConnectSubsystem::OnCanceled);

	UE_LOG(LogAnchorpointCliConnect, Display, TEXT("Anchopoint listener connected"));
}

void UAnchorpointCliConnectSubsystem::HandleMessage(const FAnchorpointConnectMessage& Message)
{
	const FString& MessageType = Message.Type;
	UE_LOG(LogAnchorpointCliConnect, Display, TEXT("Anchorpoint Source Control Provider received message: %s"), *MessageType);

	if (MessageType == TEXT("files locked") || MessageType == TEXT("files unlocked") || MessageType == TEXT("files outdated"))
	{
		AsyncTask(ENamedThreads::GameThread,
		          [this, Message]()
		          {
			          ISourceControlModule::Get().GetProvider().Execute(ISourceControlOperation::Create<FUpdateStatus>(), Message.Files);
			          RespondToMessage(Message.Id);
		          });
	}
	else if (MessageType == TEXT("project saved"))
	{
		// RespondToMessage(MessageId);

	}
	else if (MessageType == TEXT("files about to change"))
	{
		// RespondToMessage(MessageId);

	}
	else if (MessageType == TEXT("files changed"))
	{
		// RespondToMessage(MessageId);

	}
}

void UAnchorpointCliConnectSubsystem::OnOutput(const FString& Output)
{
	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Listener output: %s"), *Output);

	// TODO: Until we get onliner output, we will append the output until a valid json
	static FString JsonBuffer;
	JsonBuffer += Output;

	FAnchorpointConnectMessage Message;
	bool bParseSuccess = FJsonObjectConverter::JsonObjectStringToUStruct(JsonBuffer, &Message);

	if (bParseSuccess)
	{
		const FString ProjectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
		for (FString& File : Message.Files)
		{
			File = ProjectPath / File;
		}

		HandleMessage(Message);
		JsonBuffer.Empty();
	}
	else
	{
		// UE_LOG(LogAnchorpointCliConnect, Error, TEXT("Failed to parse message: %s"), *Output);
	}

}

void UAnchorpointCliConnectSubsystem::OnCompleted(int ReturnCode, bool bCanceling)
{
	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Listener completed with exit code %d Cannceling: %s"), ReturnCode, *LexToString(bCanceling));
}

void UAnchorpointCliConnectSubsystem::OnCanceled()
{
	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Listener canceled"));
}
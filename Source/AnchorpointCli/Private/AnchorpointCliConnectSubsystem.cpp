// Fill out your copyright notice in the Description page of Project Settings.

#include "AnchorpointCliConnectSubsystem.h"

#include <Misc/MonitoredProcess.h>

#include "AnchorpointCli.h"
#include "JsonObjectConverter.h"
#include "Misc/InteractiveProcess.h"

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
	FJsonObjectConverter::UStructToJsonObjectString(Response, JsonMessage);
	Process->SendWhenReady(JsonMessage);
}

void UAnchorpointCliConnectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const FString CommandLineExecutable = GetCliPath();
	
	TArray<FString> Args;
	Args.Add(FString::Printf(TEXT("--cwd=\"%s\""), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));
	Args.Add(TEXT("connect --name \"unreal\""));
	const FString CommandLineArgs = FString::Join(Args, TEXT(" "));
	
	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Running %s %s"), *CommandLineExecutable, *CommandLineArgs);
	Process = MakeShared<FInteractiveProcess>(CommandLineExecutable, CommandLineArgs, true);

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

void UAnchorpointCliConnectSubsystem::OnOutput(const FString& Output)
{
	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Listener output: %s"), *Output);

	// TODO: Until we get onliner output, we will append the output until a valid json
	static FString JsonBuffer;
	JsonBuffer += Output;

	FAnchorpointConnectMessage Message;
	bool bParseSuccess = FJsonObjectConverter::JsonObjectStringToUStruct(JsonBuffer, &Message);

	if(bParseSuccess)
	{
		OnMessageReceived.Broadcast(Message);
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
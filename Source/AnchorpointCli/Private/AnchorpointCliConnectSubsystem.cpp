// Fill out your copyright notice in the Description page of Project Settings.

#include "AnchorpointCliConnectSubsystem.h"

#include <ISourceControlModule.h>
#include <ISourceControlProvider.h>
#include <JsonObjectConverter.h>
#include <SourceControlOperations.h>
#include <Editor/EditorPerformanceSettings.h>
#include <Misc/InteractiveProcess.h>
#include <SourceControlHelpers.h>
#include <UnsavedAssetsTrackerModule.h>
#include <AssetRegistry/AssetRegistryModule.h>

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

	if (UEditorPerformanceSettings* EditorPerformanceSettings = GetMutableDefault<UEditorPerformanceSettings>())
	{
		EditorPerformanceSettings->bThrottleCPUWhenNotForeground = false;
		EditorPerformanceSettings->PostEditChange();
	}

	const FString CommandLineExecutable = GetCliPath();

	TArray<FString> Args;
	Args.Add(FString::Printf(TEXT("--cwd=\"%s\""), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));
	Args.Add(TEXT("connect"));
	Args.Add(TEXT("--name \"Unreal\""));
	Args.Add(TEXT("--projectSaved"));
	Args.Add(TEXT("--changedFiles"));
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
			          UE_LOG(LogAnchorpointCliConnect, Display, TEXT("Update Status requested by Cli Connect"));
			          ISourceControlModule::Get().GetProvider().Execute(ISourceControlOperation::Create<FUpdateStatus>(), Message.Files, EConcurrency::Asynchronous);
		          });
	}
	else if (MessageType == TEXT("project saved"))
	{
		TOptional<FString> Error;
		const int32 NumUnsaved = FUnsavedAssetsTrackerModule::Get().GetUnsavedAssets().Num();
		if (NumUnsaved > 0)
		{
			Error = FString::Printf(TEXT("There are %d unsaved assets"), NumUnsaved);
		}

		RespondToMessage(Message.Id, Error);
	}
	else if (MessageType == TEXT("files about to change"))
	{
		if(bSyncInProgress)
		{
			RespondToMessage(Message.Id, FString(TEXT("Sync already in progress")));
			return;
		}

		TArray<FString> PackageToReload;
		for (const FString& File : Message.Files)
		{
			FString PackageName;
			if (FPackageName::TryConvertFilenameToLongPackageName(File, PackageName, nullptr))
			{
				PackageToReload.Add(PackageName);
			}
		}

		bSyncInProgress = true;

		AsyncTask(ENamedThreads::GameThread,
		          [this, PackageToReload, Message]()
		          {
			          RespondToMessage(Message.Id);
			          auto WaitForSync = [this](const TArray<FString>& PackageFilenames) { return UpdateSync(PackageFilenames); };
			          USourceControlHelpers::ApplyOperationAndReloadPackages(PackageToReload, WaitForSync, true, false);
		          });
	}
	else if (MessageType == TEXT("files changed"))
	{
		if(!bSyncInProgress)
		{
			RespondToMessage(Message.Id, FString(TEXT("Sync was not started")));
			return;
		}

		bSyncInProgress = false;
		RespondToMessage(Message.Id);
	}
}

void UAnchorpointCliConnectSubsystem::OnOutput(const FString& Output)
{
	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Listener output: %s"), *Output);

	FAnchorpointConnectMessage Message;

	if (FJsonObjectConverter::JsonObjectStringToUStruct(Output, &Message))
	{
		// Anchorpoint CLI sends relative paths, so we need to convert them to full paths
		const FString ProjectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
		for (FString& File : Message.Files)
		{
			File = ProjectPath / File;
		}

		HandleMessage(Message);
	}
	else
	{
		UE_LOG(LogAnchorpointCliConnect, Error, TEXT("Failed to parse message: %s"), *Output);
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

bool UAnchorpointCliConnectSubsystem::UpdateSync(const TArray<FString>& PackageFilenames)
{
	float TimeElapsed = 0.0f;

	constexpr float TimeOut = 600.0f;
	constexpr float TimeStep = 1.0f;

	while(bSyncInProgress)
	{
		FPlatformProcess::Sleep(TimeStep);
		TimeElapsed += TimeStep;
		UE_LOG(LogAnchorpointCliConnect, Display, TEXT("Sync in progress for %f seconds"), TimeElapsed);

		if(TimeElapsed > TimeOut)
		{
			UE_LOG(LogAnchorpointCliConnect, Error, TEXT("Sync timed out"));
			bSyncInProgress = false;
		}
	}

	return true;
}
#include "AnchorpointCliConnectSubsystem.h"

#include <ISourceControlModule.h>
#include <ISourceControlProvider.h>
#include <JsonObjectConverter.h>
#include <Misc/InteractiveProcess.h>
#include <SourceControlHelpers.h>
#include <SourceControlOperations.h>
#include <UnsavedAssetsTrackerModule.h>

#include "AnchorpointCli.h"
#include "AnchorpointCliLog.h"

void UAnchorpointCliConnectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UAnchorpointCliConnectSubsystem::Tick), 5.0f);
}

void UAnchorpointCliConnectSubsystem::EstablishConnection()
{
	ISourceControlProvider& Provider = ISourceControlModule::Get().GetProvider();
	if(Provider.GetName().ToString() != TEXT("Anchorpoint") || !Provider.IsEnabled())
	{
		// We should not connect if the current provider is not Anchorpoint
		return;
	}

	if(Process)
	{
		// Process already running and connection is established 
		return;
	}

	const FString CommandLineExecutable = FAnchorpointCliModule::Get().GetCliPath();

	TArray<FString> Args;
	Args.Add(FString::Printf(TEXT("--cwd=\"%s\""), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));
	Args.Add(TEXT("connect"));
	Args.Add(TEXT("--name \"Unreal\""));
	Args.Add(TEXT("--projectSaved"));
	Args.Add(TEXT("--changedFiles"));
	const FString CommandLineArgs = FString::Join(Args, TEXT(" "));

	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Running %s %s"), *CommandLineExecutable, *CommandLineArgs);
	Process = MakeShared<FInteractiveProcess>(CommandLineExecutable, CommandLineArgs, true, true);

	if (!Process)
	{
		UE_LOG(LogAnchorpointCli, Error, TEXT("Failed to create process"));
		return;
	}

	const bool bLaunchSuccess = Process->Launch();
	if (!bLaunchSuccess)
	{
		UE_LOG(LogAnchorpointCli, Error, TEXT("Failed to launch process"));
		return;
	}

	Process->OnOutput().BindUObject(this, &UAnchorpointCliConnectSubsystem::OnOutput);
	Process->OnCompleted().BindUObject(this, &UAnchorpointCliConnectSubsystem::OnCompleted);
	Process->OnCanceled().BindUObject(this, &UAnchorpointCliConnectSubsystem::OnCanceled);

	UE_LOG(LogAnchorpointCli, Display, TEXT("Anchorpoint listener connected"));
}

bool UAnchorpointCliConnectSubsystem::Tick(const float InDeltaTime)
{
	EstablishConnection();

	return true;
}

void UAnchorpointCliConnectSubsystem::RefreshStatus(const FAnchorpointConnectMessage& Message)
{
	AsyncTask(ENamedThreads::GameThread,
	          [this, Message]()
	          {
		          UE_LOG(LogAnchorpointCli, Display, TEXT("Update Status requested by Cli Connect"));
		          ISourceControlModule::Get().GetProvider().Execute(ISourceControlOperation::Create<FUpdateStatus>(), Message.Files, EConcurrency::Asynchronous);
	          });
}

void UAnchorpointCliConnectSubsystem::CheckProjectSaveStatus(const FAnchorpointConnectMessage& Message)
{
	TOptional<FString> Error;
	const int32 NumUnsaved = FUnsavedAssetsTrackerModule::Get().GetUnsavedAssets().Num();
	if (NumUnsaved > 0)
	{
		Error = FString::Printf(TEXT("There are %d unsaved assets"), NumUnsaved);
	}

	RespondToMessage(Message.Id, Error);
}

void UAnchorpointCliConnectSubsystem::StartSync(const FAnchorpointConnectMessage& Message)
{
	if (bSyncInProgress)
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

void UAnchorpointCliConnectSubsystem::StopSync(const FAnchorpointConnectMessage& Message)
{
	if (!bSyncInProgress)
	{
		RespondToMessage(Message.Id, FString(TEXT("Sync was not started")));
		return;
	}

	bSyncInProgress = false;
	RespondToMessage(Message.Id);
}

void UAnchorpointCliConnectSubsystem::HandleMessage(const FAnchorpointConnectMessage& Message)
{
	const FString& MessageType = Message.Type;
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Anchorpoint Source Control Provider received message: %s"), *MessageType);

	if (MessageType == TEXT("files locked") || MessageType == TEXT("files unlocked") || MessageType == TEXT("files outdated"))
	{
		RefreshStatus(Message);
	}
	else if (MessageType == TEXT("project saved"))
	{
		CheckProjectSaveStatus(Message);
	}
	else if (MessageType == TEXT("files about to change"))
	{
		StartSync(Message);
	}
	else if (MessageType == TEXT("files changed"))
	{
		StopSync(Message);
	}
}

void UAnchorpointCliConnectSubsystem::RespondToMessage(const FString& Id, TOptional<FString> Error)
{
	const FString ErrorString = Error.Get(TEXT(""));
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Responding to message %s with error %s"), *Id, *ErrorString);

	FAnchorpointCliConnectResponse Response;
	Response.Id = Id;
	Response.Error = ErrorString;

	FString JsonMessage;
	FJsonObjectConverter::UStructToJsonObjectString<FAnchorpointCliConnectResponse>(Response, JsonMessage, 0, 0, 0, nullptr, false);

	Process->SendWhenReady(JsonMessage);
}

void UAnchorpointCliConnectSubsystem::OnOutput(const FString& Output)
{
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Listener output: %s"), *Output);

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
		UE_LOG(LogAnchorpointCli, Error, TEXT("Failed to parse message: %s"), *Output);
	}
}

void UAnchorpointCliConnectSubsystem::OnCompleted(int ReturnCode, bool bCanceling)
{
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Listener completed with exit code %d Cannceling: %s"), ReturnCode, *LexToString(bCanceling));

	if(Process)
	{
		Process.Reset();
	}
}

void UAnchorpointCliConnectSubsystem::OnCanceled()
{
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Listener canceled"));
}

bool UAnchorpointCliConnectSubsystem::UpdateSync(const TArray<FString>& PackageFilenames)
{
	float TimeElapsed = 0.0f;

	constexpr float TimeOut = 1800.0f; // 30 minutes
	constexpr float TimeStep = 1.0f;

	while (bSyncInProgress)
	{
		FPlatformProcess::Sleep(TimeStep);
		TimeElapsed += TimeStep;
		UE_LOG(LogAnchorpointCli, VeryVerbose, TEXT("Sync in progress for %f seconds"), TimeElapsed);

		if (TimeElapsed > TimeOut)
		{
			UE_LOG(LogAnchorpointCli, Error, TEXT("Sync timed out"));
			bSyncInProgress = false;
		}
	}

	return true;
}
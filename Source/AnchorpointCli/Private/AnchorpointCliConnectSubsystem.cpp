#include "AnchorpointCliConnectSubsystem.h"

#include <Async/Async.h>
#include <ISourceControlModule.h>
#include <ISourceControlProvider.h>
#include <JsonObjectConverter.h>
#include <Misc/InteractiveProcess.h>
#include <SourceControlHelpers.h>
#include <SourceControlOperations.h>
#include <UnsavedAssetsTrackerModule.h>
#include <FileHelpers.h>

#include "AnchorpointCli.h"
#include "AnchorpointCliLog.h"
#include "AnchorpointCliOperations.h"
#include "Dialogs/Dialogs.h"

TOptional<FAnchorpointStatus> UAnchorpointCliConnectSubsystem::GetCachedStatus() const
{
	if (!bCanUseStatusCache)
	{
		return {};
	}

	return StatusCache;
}

void UAnchorpointCliConnectSubsystem::UpdateStatusCacheIfPossible(const FAnchorpointStatus& Status)
{
	if (!bCanUseStatusCache)
	{
		return;
	}

	StatusCache = Status;
}

void UAnchorpointCliConnectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UAnchorpointCliConnectSubsystem::Tick), 5.0f);
}

void UAnchorpointCliConnectSubsystem::TickConnection()
{
	ISourceControlProvider& Provider = ISourceControlModule::Get().GetProvider();
	const bool bUsesAnchorpoint = Provider.GetName().ToString() == TEXT("Anchorpoint") && Provider.IsEnabled();
	if (!bUsesAnchorpoint)
	{
		if (Process)
		{
			UE_LOG(LogAnchorpointCli, Warning, TEXT("Disconnecting listener because the current provider is not Anchorpoint"));
			Process->Cancel(true);
		}

		// We should not connect if the current provider is not Anchorpoint
		return;
	}

	TValueOrError<FString, FString> CurrentUser = AnchorpointCliOperations::GetCurrentUser();
	if (CurrentUser.HasError())
	{
		// We should not connect until we have a valid user
		return;
	}

	if (Process)
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
	TickConnection();

	return true;
}

void UAnchorpointCliConnectSubsystem::RefreshStatus(const FAnchorpointConnectMessage& Message)
{
	TArray<FString> FilesToUpdate;
	if (bCanUseStatusCache)
	{
		// When using status caching, we always want to run the status command for the whole project (no specific files) and clear our cache
		StatusCache.Reset();
	}
	else
	{
		FilesToUpdate = Message.Files;
	}

	AsyncTask(ENamedThreads::GameThread,
	          [this, FilesToUpdate]()
	          {
		          UE_LOG(LogAnchorpointCli, Display, TEXT("Update Status requested by Cli Connect"));
		          ISourceControlModule::Get().GetProvider().Execute(ISourceControlOperation::Create<FUpdateStatus>(), FilesToUpdate, EConcurrency::Asynchronous);
	          });
}

TOptional<FString> UAnchorpointCliConnectSubsystem::CheckProjectSaveStatus(const TArray<FString>& Files)
{
	FUnsavedAssetsTrackerModule& UnsavedTracker = FUnsavedAssetsTrackerModule::Get();
	TOptional<FString> Error;

	if (Files.IsEmpty())
	{
		// No specific files specified, checking everything
		const int32 NumUnsaved = UnsavedTracker.GetUnsavedAssets().Num();
		if (NumUnsaved > 0)
		{
			Error = FString::Printf(TEXT("%d unsaved asset(s) present"), NumUnsaved);
		}
	}
	else
	{
		FString ErrorMessage;
		for (const FString& File : Files)
		{
			if (UnsavedTracker.IsAssetUnsaved(File))
			{
				ErrorMessage.Appendf(TEXT("%s is unsaved\n"), *AnchorpointCliOperations::ConvertFullPathToApInternal(File));
			}
		}

		if (!ErrorMessage.IsEmpty())
		{
			Error = ErrorMessage;
		}
	}

	return Error;
}

void UAnchorpointCliConnectSubsystem::StartSync(const FAnchorpointConnectMessage& Message)
{
	if (bSyncInProgress)
	{
		RespondToMessage(Message.Id, FString(TEXT("Sync already in progress")));
		return;
	}

	bSyncInProgress = true;

	AsyncTask(ENamedThreads::GameThread,
	          [this, Message]()
	          {
		          PerformSync(Message);
	          });
}

void UAnchorpointCliConnectSubsystem::PerformSync(const FAnchorpointConnectMessage& Message)
{
	check(IsInGameThread());

	TArray<FString> PackageToReload;
	for (const FString& File : Message.Files)
	{
		FString PackageName;
		if (FPackageName::TryConvertFilenameToLongPackageName(File, PackageName, nullptr))
		{
			PackageToReload.Add(PackageName);

		}
	}

	FAssetData CurrentWorldAsset;
	for (const FString& PackageName : PackageToReload)
	{
		UPackage* Package = FindPackage(nullptr, *PackageName);
		if (UObject* Asset = Package ? Package->FindAssetInPackage() : nullptr)
		{
			if (Asset->IsPackageExternal())
			{
				if (Asset->GetWorld() && Asset->GetWorld() == GWorld && Asset->GetWorld()->GetPackage())
				{
					UE_LOG(LogAnchorpointCli, Verbose, TEXT("Current world will be reloaded because of %s"), *PackageName);
					CurrentWorldAsset = FAssetData(GWorld);
				}
			}
		}
	}

	if (CurrentWorldAsset.IsValid())
	{
		UE_LOG(LogAnchorpointCli, Verbose, TEXT("Switching to a blank world"));
		GEditor->CreateNewMapForEditing();
	}

	RespondToMessage(Message.Id);

	auto WaitForSync = [this](const TArray<FString>& PackageFilenames) { return UpdateSync(PackageFilenames); };
	USourceControlHelpers::ApplyOperationAndReloadPackages(PackageToReload, WaitForSync, true, false);

	if (CurrentWorldAsset.IsValid())
	{
		UE_LOG(LogAnchorpointCli, Verbose, TEXT("Waiting for user input to reload the initial world"));
		const FText Title = NSLOCTEXT("Anchorpoint", "ReloadPromptTitle", "Active Level Changed");
		const FText Content = NSLOCTEXT("Anchorpoint", "ReloadPromptContent", "Anchorpoint changed your active level. Reload the level to see the changes.");
		FString CurrentWorldPackageName = CurrentWorldAsset.PackageName.ToString();

		TSharedRef<SWindow> PopupWindow = OpenMsgDlgInt_NonModal(EAppMsgType::Ok,
		                                                         Content,
		                                                         Title,
		                                                         FOnMsgDlgResult::CreateLambda([CurrentWorldPackageName](const TSharedRef<SWindow>&, EAppReturnType::Type Choice)
		                                                         {
			                                                         UE_LOG(LogAnchorpointCli, Verbose, TEXT("Reloading initial world"));
			                                                         FEditorFileUtils::LoadMap(CurrentWorldPackageName, false, false);
		                                                         }));
		PopupWindow->ShowWindow();

		FSlateApplication::Get().OnPostTick().AddLambda(
			[WeakWindow = PopupWindow.ToWeakPtr()](float DeltaTime)
			{
				if (TSharedPtr<SWindow> Window = WeakWindow.Pin())
				{
					Window->HACK_ForceToFront();
				}
			});
	}
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

	if (MessageType == TEXT("files locked") || MessageType == TEXT("files unlocked") || MessageType == TEXT("files outdated") || MessageType == TEXT("files updated"))
	{
		RefreshStatus(Message);
	}
	else if (MessageType == TEXT("project saved"))
	{
		const TOptional<FString> Error = CheckProjectSaveStatus(Message.Files);
		RespondToMessage(Message.Id, Error);
	}
	else if (MessageType == TEXT("files about to change"))
	{
		// Note: Pulling can be dangerous, so any unsaved work should prevent the operation.
		const TOptional<FString> Error = CheckProjectSaveStatus({});
		if (Error.IsSet())
		{
			RespondToMessage(Message.Id, Error);
		}
		else
		{
			StartSync(Message);
		}
	}
	else if (MessageType == TEXT("files changed"))
	{
		StopSync(Message);

		RefreshStatus(Message);
	}
	else if (MessageType == TEXT("project opened"))
	{
		bCanUseStatusCache = true;

		RefreshStatus(Message);
	}
	else if (MessageType == TEXT("project closed"))
	{
		bCanUseStatusCache = false;

		RefreshStatus(Message);
	}
	else if (MessageType == TEXT("project dirty"))
	{
		StatusCache.Reset();

		RefreshStatus(Message);
	}
	else
	{
		UE_LOG(LogAnchorpointCli, Warning, TEXT("Unknown message type: %s"), *MessageType);
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

	if (Output.TrimStartAndEnd().EndsWith(TEXT("disconnected"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogAnchorpointCli, Warning, TEXT("CLI connection lost"));

		if (Process)
		{
			Process->Cancel(true);
		}

		return;
	}

	FAnchorpointConnectMessage Message;
	if (FJsonObjectConverter::JsonObjectStringToUStruct(Output, &Message))
	{
		// Anchorpoint CLI sends relative paths, so we need to convert them to full paths
		for (FString& File : Message.Files)
		{
			File = AnchorpointCliOperations::ConvertApInternalToFull(File);
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

	if (Process)
	{
		Process.Reset();
	}
}

void UAnchorpointCliConnectSubsystem::OnCanceled()
{
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Listener canceled"));

	if (Process)
	{
		Process.Reset();
	}
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
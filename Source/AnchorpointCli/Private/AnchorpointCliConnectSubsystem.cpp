// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointCliConnectSubsystem.h"

#include <Async/Async.h>
#include <Dialogs/Dialogs.h>
#include <FileHelpers.h>
#include <Framework/Notifications/NotificationManager.h>
#include <ISourceControlModule.h>
#include <ISourceControlProvider.h>
#include <JsonObjectConverter.h>
#include <SourceControlHelpers.h>
#include <SourceControlOperations.h>
#include <UnsavedAssetsTrackerModule.h>
#include <Widgets/Notifications/SNotificationList.h>
#include <Kismet/KismetStringLibrary.h>

#include "AnchorpointCli.h"
#include "AnchorpointCliLog.h"
#include "AnchorpointCliOperations.h"
#include "AnchorpointCliProcess.h"

TOptional<TArray<FAnchorpointConnectMessage>> FAnchorpointConnectMessage::ParseStringToMessages(const FString& InString)
{
	TArray<FAnchorpointConnectMessage> Messages;
	TArray<FString> StringMessages = UKismetStringLibrary::ParseIntoArray(InString, TEXT("\n"), true);
	for (const FString& StringMessage : StringMessages)
	{
		FScopedCategoryAndVerbosityOverride LogOverride(TEXT("LogJson"), ELogVerbosity::Error);

		FAnchorpointConnectMessage Message;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(StringMessage, &Message))
		{
			// Anchorpoint CLI sends relative paths, so we need to convert them to full paths
			for (FString& File : Message.Files)
			{
				File = AnchorpointCliOperations::ConvertApInternalToFull(File);
			}

			Messages.Add(Message);
		}
		else
		{
			// NOTE: If any of the messages fails to parse, we abort the whole operation.
			// This is done to prevent deletion of the output data when it contains invalid data.
			return {};
		}
	}

	return Messages;
}

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

bool UAnchorpointCliConnectSubsystem::IsConnected() const
{
	return Process && Process->IsRunning();
}

void UAnchorpointCliConnectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UAnchorpointCliConnectSubsystem::Tick), 30.0f);

	FAnchorpointCliModule::Get().OnAnchorpointConnected.AddUObject(this, &UAnchorpointCliConnectSubsystem::OnAnchorpointProviderConnected);

	FakeAnchorpointCliMessage = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("FakeAnchorpointCliMessage"),
		TEXT("Mimics a message from the Anchorpoint CLI"),
		FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateUObject(this, &UAnchorpointCliConnectSubsystem::OnFakeAnchorpointCliMessage),
		ECVF_Default
	);
}

void UAnchorpointCliConnectSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (FakeAnchorpointCliMessage)
	{
		IConsoleManager::Get().UnregisterConsoleObject(FakeAnchorpointCliMessage);
	}
}

void UAnchorpointCliConnectSubsystem::TickConnection()
{
	ISourceControlProvider& Provider = ISourceControlModule::Get().GetProvider();
	const bool bUsesAnchorpoint = Provider.IsEnabled() && Provider.GetName().ToString().Contains(TEXT("Anchorpoint"));
	if (!bUsesAnchorpoint)
	{
		if (Process)
		{
			UE_LOG(LogAnchorpointCli, Warning, TEXT("Disconnecting listener because the current provider is not Anchorpoint"));

			ToastConnectionState(false);

			Process->Cancel();
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

	Process = MakeShared<FAnchorpointCliProcess>();
	const bool bLaunchSuccess = Process->Launch(CommandLineExecutable, CommandLineArgs);
	if (!bLaunchSuccess)
	{
		UE_LOG(LogAnchorpointCli, Error, TEXT("Failed to launch process"));
		return;
	}

	Process->OnProcessUpdated.AddUObject(this, &UAnchorpointCliConnectSubsystem::OnProcessUpdated);
	Process->OnProcessEnded.AddUObject(this, &UAnchorpointCliConnectSubsystem::OnProcessEnded);

	UE_LOG(LogAnchorpointCli, Display, TEXT("Anchorpoint listener connected"));

	ToastConnectionState(true);
}

bool UAnchorpointCliConnectSubsystem::Tick(const float InDeltaTime)
{
	TickConnection();

	return true;
}

void UAnchorpointCliConnectSubsystem::RefreshStatus(const FAnchorpointConnectMessage& Message)
{
	TSharedRef<FUpdateStatus> UpdateRequest = ISourceControlOperation::Create<FUpdateStatus>();
	TArray<FString> FilesToUpdate;
	if (bCanUseStatusCache)
	{
		// When using status caching, we always want to run a force status command for the whole project (no specific files) and clear our cache
		UpdateRequest->SetForceUpdate(true);
	}
	else
	{
		FilesToUpdate = Message.Files;
	}

	AsyncTask(ENamedThreads::GameThread,
	          [this, UpdateRequest, FilesToUpdate]()
	          {
		          UE_LOG(LogAnchorpointCli, Display, TEXT("Update Status requested by Cli Connect"));
		          ISourceControlModule::Get().GetProvider().Execute(UpdateRequest, FilesToUpdate, EConcurrency::Asynchronous);
	          });
}

TOptional<FString> UAnchorpointCliConnectSubsystem::CheckProjectSaveStatus(const TArray<FString>& Files)
{
	FString ErrorMessage;

	FUnsavedAssetsTrackerModule& UnsavedTracker = FUnsavedAssetsTrackerModule::Get();
	for (const FString& UnsavedAsset : UnsavedTracker.GetUnsavedAssets())
	{
		if (Files.IsEmpty() || Files.Contains(UnsavedAsset))
		{
			ErrorMessage.Appendf(TEXT("%s is unsaved\n"), *AnchorpointCliOperations::ConvertFullPathToApInternal(UnsavedAsset));
		}
	}

	return !ErrorMessage.IsEmpty() ? ErrorMessage : TOptional<FString>();
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

void UAnchorpointCliConnectSubsystem::OnAnchorpointProviderConnected()
{
	// Extra ticks in case a relevant event happened.
	TickConnection();
}

void UAnchorpointCliConnectSubsystem::HandleMessage(const FAnchorpointConnectMessage& Message)
{
	const FString& MessageType = Message.Type;
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Anchorpoint Source Control Provider received message: %s"), *MessageType);

	OnPreMessageHandled.Broadcast(Message);

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

void UAnchorpointCliConnectSubsystem::OnProcessUpdated()
{
	FString StringOutput;
	TArray<uint8> BinaryOutput;

	Process->GetStdOutData(StringOutput, BinaryOutput);

	if (StringOutput.IsEmpty())
	{
		return;
	}

	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Listener output: %s"), *StringOutput);

	if (StringOutput.TrimStartAndEnd().EndsWith(TEXT("disconnected"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogAnchorpointCli, Warning, TEXT("CLI connection lost"));

		ToastConnectionState(false);

		if (Process)
		{
			Process->Cancel();
		}

		return;
	}

	TOptional<TArray<FAnchorpointConnectMessage>> ParsedMessages = FAnchorpointConnectMessage::ParseStringToMessages(StringOutput);

	if (ParsedMessages.IsSet())
	{
		// We only clear the stderr data if we successfully parsed the message,
		// otherwise we might lose important information when longer messages are split into multiple parts
		Process->ClearStdOutData();

		UE_LOG(LogAnchorpointCli, Verbose, TEXT("Anchorpoint Source Control Provider parsed %s messages"), *LexToString(ParsedMessages->Num()));
		for (const FAnchorpointConnectMessage& Message : ParsedMessages.GetValue())
		{
			HandleMessage(Message);
		}
	}
	else
	{
		UE_LOG(LogAnchorpointCli, VeryVerbose, TEXT("Failed to parse message: %s"), *StringOutput);
	}
}

void UAnchorpointCliConnectSubsystem::OnProcessEnded()
{
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Listener canceled"));

	ToastConnectionState(false);

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

void UAnchorpointCliConnectSubsystem::OnFakeAnchorpointCliMessage(const TArray<FString>& Params, UWorld* InWorld, FOutputDevice& Ar)
{
	const FString FakeMessage = FString::Join(Params, TEXT(" "));

	UE_LOG(LogAnchorpointCli, Warning, TEXT("Simulating fake message: %s"), *FakeMessage);

	FAnchorpointConnectMessage Message;
	if (FJsonObjectConverter::JsonObjectStringToUStruct(FakeMessage, &Message))
	{
		HandleMessage(Message);
	}
	else
	{
		UE_LOG(LogAnchorpointCli, Error, TEXT("Failed to parse fake message: %s"), *FakeMessage);
	}
}

void UAnchorpointCliConnectSubsystem::ToastConnectionState(bool bConnected)
{
	const FText ConnectText = NSLOCTEXT("Anchorpoint", "AnchorpointCliConnected", "Anchorpoint Unreal Engine Integration activated");
	const FText DisconnectText = NSLOCTEXT("Anchorpoint", "AnchorpointCliDisconnected", "Anchorpoint Unreal Engine Integration deactivated");

	const FSlateBrush* ConnectBrush = FAppStyle::Get().GetBrush("Icons.SuccessWithColor.Large");
	const FSlateBrush* DisconnectBrush = FAppStyle::Get().GetBrush("Icons.ErrorWithColor.Large");

	FNotificationInfo* Info = new FNotificationInfo(bConnected ? ConnectText : DisconnectText);
	Info->ExpireDuration = 2.0f;
	Info->Image = bConnected ? ConnectBrush : DisconnectBrush;

	FSlateNotificationManager::Get().QueueNotification(Info);
}
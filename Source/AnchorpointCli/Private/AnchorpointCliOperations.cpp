// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointCliOperations.h"

#include <Misc/FileHelper.h>
#include <Dom/JsonObject.h>
#include <Dom/JsonValue.h>
#include <Framework/Notifications/NotificationManager.h>
#include <Widgets/Notifications/SNotificationList.h>
#include <ISourceControlModule.h>
#include <ISourceControlProvider.h>
#include <SourceControlOperations.h>

#include "AnchorpointCli.h"
#include "AnchorpointCliConnectSubsystem.h"
#include "AnchorpointCliLog.h"
#include "AnchorpointCliCommands.h"
#include "AnchorpointCliProcess.h"

bool AnchorpointCliOperations::IsInstalled()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::IsInstalled);

	const FString CliPath = FAnchorpointCliModule::Get().GetCliPath();
	return !CliPath.IsEmpty() && FPaths::FileExists(CliPath);
}

void AnchorpointCliOperations::ShowInAnchorpoint(const FString& InPath)
{
	const FString ApplicationPath = FAnchorpointCliModule::Get().GetApplicationPath();
	FPlatformProcess::CreateProc(*ApplicationPath, *InPath, true, true, false, nullptr, 0, nullptr, nullptr);
}

FString AnchorpointCliOperations::GetRepositoryRootPath()
{
	static FString RepositoryRootPath;
	if (!RepositoryRootPath.IsEmpty())
	{
		return RepositoryRootPath;
	}

	// Searching for the .approject file in the project directory and all its parent directories
	FString SearchPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

	while (!SearchPath.IsEmpty())
	{
		TArray<FString> AnchorpointProjectFile;
		IFileManager::Get().FindFiles(AnchorpointProjectFile, *(SearchPath / TEXT("*.approj")), true, false);

		if (!AnchorpointProjectFile.IsEmpty())
		{
			RepositoryRootPath = SearchPath;
			if (!RepositoryRootPath.EndsWith("/"))
			{
				RepositoryRootPath += "/";
			}

			return RepositoryRootPath;
		}

		SearchPath = FPaths::GetPath(SearchPath);
	}

	return {};
}

bool AnchorpointCliOperations::IsUnderRepositoryPath(const FString& InPath)
{
	FString RootFolder = GetRepositoryRootPath();
	return FPaths::IsUnderDirectory(InPath, RootFolder);
}

FString AnchorpointCliOperations::ConvertFullPathToApInternal(const FString& InFullPath)
{
	FString Path = InFullPath;
	FPaths::CollapseRelativeDirectories(Path);

	const FString RootFolder = GetRepositoryRootPath();
	Path = Path.Replace(*RootFolder, TEXT(""), ESearchCase::CaseSensitive);

	return Path;
}

FString AnchorpointCliOperations::ConvertApInternalToFull(const FString& InRelativePath)
{
	FString Path = GetRepositoryRootPath() / InRelativePath;
	FPaths::NormalizeDirectoryName(Path);

	return Path;
}

FString AnchorpointCliOperations::ConvertFullPathToProjectRelative(const FString& InPath)
{
	FString Result = InPath;
	FPaths::MakePathRelativeTo(Result, *FPaths::ProjectDir());
	return Result;
}

TValueOrError<FString, FString> AnchorpointCliOperations::GetCurrentUser()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::GetCurrentUser);

	static FString CurrentUser;
	if (!CurrentUser.IsEmpty())
	{
		// Return the cached value if we have run this before
		return MakeValue(CurrentUser);
	}

	FString UserCommand = TEXT("user list");
	FCliResult ProcessOutput = AnchorpointCliCommands::RunApCommand(UserCommand);
	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.GetBestError());
	}

	TArray<TSharedPtr<FJsonValue>> Users = ProcessOutput.OutputAsJsonArray();
	for (const TSharedPtr<FJsonValue>& User : Users)
	{
		TSharedPtr<FJsonObject> UserObject = User->AsObject();
		if (UserObject->HasField(TEXT("current")) && UserObject->GetBoolField(TEXT("current")))
		{
			CurrentUser = UserObject->GetStringField(TEXT("email"));
			UE_LOG(LogAnchorpointCli, Log, TEXT("Current user found: %s"), *CurrentUser);
			return MakeValue(CurrentUser);
		}
	}

	return MakeError(TEXT("Could not find current user"));
}

TValueOrError<FAnchorpointStatus, FString> AnchorpointCliOperations::GetStatus(const TArray<FString>& InFiles, bool bForced)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::GetStatus);

	UAnchorpointCliConnectSubsystem* ConnectSubsystem = GEditor->GetEditorSubsystem<UAnchorpointCliConnectSubsystem>();

	// Any non-forced status update can access the cached status for a faster response if available (even if an update is in progress)
	if (TOptional<FAnchorpointStatus> CachedStatus = !bForced ? ConnectSubsystem->GetCachedStatus() : TOptional<FAnchorpointStatus>())
	{
		return MakeValue(CachedStatus.GetValue());
	}

	// NOTE: Lock is needed to prevent multiple threads from updating the status at the same time
	static FCriticalSection StatusUpdateLock;
	FScopeLock ScopeLock(&StatusUpdateLock);

	if (TOptional<FAnchorpointStatus> CachedStatus = !bForced ? ConnectSubsystem->GetCachedStatus() : TOptional<FAnchorpointStatus>())
	{
		//NOTE: After the lock was acquired, the status may have changed, so it's worth checking again
		return MakeValue(CachedStatus.GetValue());
	}

	TArray<FString> StatusParams;
	StatusParams.Add(TEXT("status"));

	if (!InFiles.IsEmpty())
	{
		StatusParams.Add(TEXT("--paths"));
		for (const FString& File : InFiles)
		{
			if (AnchorpointCliOperations::IsUnderRepositoryPath(File))
			{
				StatusParams.Add(FString::Printf(TEXT("\"%s\""), *AnchorpointCliOperations::ConvertFullPathToApInternal(File)));
			}
		}
	}

	FString StatusCommand = FString::Join(StatusParams, TEXT(" "));
	FCliResult ProcessOutput = AnchorpointCliCommands::RunApCommand(StatusCommand);
	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.GetBestError());
	}

	TSharedPtr<FJsonObject> Object = ProcessOutput.OutputAsJsonObject();
	if (!Object.IsValid())
	{
		return MakeError(FString::Printf(TEXT("Failed to parse output to JSON.")));
	}

	FString Error;
	if (Object->TryGetStringField(TEXT("error"), Error))
	{
		return MakeError(FString::Printf(TEXT("CLI error: %s"), *Error));
	}

	FAnchorpointStatus Status = FAnchorpointStatus::FromJson(Object.ToSharedRef());

	if (InFiles.IsEmpty())
	{
		// Only a project-wide (no specific files provided) status update should be cached
		ConnectSubsystem->UpdateStatusCacheIfPossible(Status);
	}

	return MakeValue(Status);
}

TValueOrError<FString, FString> AnchorpointCliOperations::DisableAutoLock()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::DisableAutoLock);

	// Note: Running `config set --key key --value value` is not directly supported, but this is the format the unreal the ini parser expects
	// Alternatively, (1) we could run the command directly skipping the ini file or (2) implement key-value pairs in the parser
	FString AutoLockSetCommand = FString::Printf(TEXT("config set --key git-auto-lock --value false"));

	FCliResult AutoLockSetOutput = AnchorpointCliCommands::RunApCommand(AutoLockSetCommand);

	if (!AutoLockSetOutput.DidSucceed())
	{
		return MakeError(AutoLockSetOutput.GetBestError());
	}

	return MakeValue(TEXT("Success"));
}

TValueOrError<FString, FString> AnchorpointCliOperations::LockFiles(const TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::LockFiles);

	TArray<FString> LockParams;
	LockParams.Add(TEXT("lock create"));
	LockParams.Add(TEXT("--git"));
	LockParams.Add(TEXT("--keep"));
	LockParams.Add(TEXT("--files"));

	for (const FString& File : InFiles)
	{
		LockParams.Add(FString::Printf(TEXT("\"%s\""), *AnchorpointCliOperations::ConvertFullPathToApInternal(File)));
	}

	FString LockCommand = FString::Join(LockParams, TEXT(" "));
	FCliResult ProcessOutput = AnchorpointCliCommands::RunApCommand(LockCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.GetBestError());
	}

	return MakeValue(TEXT("Success"));
}

TValueOrError<FString, FString> AnchorpointCliOperations::UnlockFiles(const TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::UnlockFiles);

	TArray<FString> UnlockParams;
	UnlockParams.Add(TEXT("lock remove"));
	UnlockParams.Add(TEXT("--files"));

	for (const FString& File : InFiles)
	{
		UnlockParams.Add(FString::Printf(TEXT("\"%s\""), *AnchorpointCliOperations::ConvertFullPathToApInternal(File)));
	}

	FString UnlockCommand = FString::Join(UnlockParams, TEXT(" "));
	FCliResult ProcessOutput = AnchorpointCliCommands::RunApCommand(UnlockCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.GetBestError());
	}

	return MakeValue(TEXT("Success"));
}

TValueOrError<FString, FString> AnchorpointCliOperations::Revert(const TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::Revert);

	if (InFiles.IsEmpty())
	{
		return MakeValue(TEXT("No files to revert"));
	}

	TArray<FString> RevertParams;
	RevertParams.Add(TEXT("revert"));
	RevertParams.Add(TEXT("--files"));

	for (const FString& File : InFiles)
	{
		RevertParams.Add(FString::Printf(TEXT("\"%s\""), *AnchorpointCliOperations::ConvertFullPathToApInternal(File)));
	}

	FString RevertCommand = FString::Join(RevertParams, TEXT(" "));
	FCliResult ProcessOutput = AnchorpointCliCommands::RunApCommand(RevertCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.GetBestError());
	}

	return MakeValue(TEXT("Success"));
}

TValueOrError<FString, FString> AnchorpointCliOperations::DeleteFiles(const TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::DeleteFiles);

	for (const FString& File : InFiles)
	{
		IFileManager::Get().Delete(*File, false, true, true);
	}

	return MakeValue(TEXT("Success"));
}

bool IsSubmitFinished(const TSharedRef<FAnchorpointCliProcess>& InProcess, const FString& InProcessOutput)
{
	if (InProcessOutput.Contains(TEXT("Uploading LFS objects"))
		|| InProcessOutput.Contains(TEXT("Pushing Git Changes"))
		|| InProcessOutput.Contains(TEXT("Uploading Files")))
	{
		UE_LOG(LogAnchorpointCli, Verbose, TEXT("Special log messages found. Marking submit as finished early."));

		const FText ProgressText = NSLOCTEXT("Anchorpoint", "CheckInSuccess", "Background push started");
		const FText FinishText = NSLOCTEXT("Anchorpoint", "CheckInFinish", "Background push completed");
		AnchorpointCliOperations::MonitorProcessWithNotification(InProcess, ProgressText, FinishText);

		// Note: In case we detach the process early, we will queue a status update at the end of the push.
		// This is particularly important when the CLI is not connected because we won't receive any status updates.
		InProcess->OnProcessEnded.AddLambda([]()
		{
			AsyncTask(ENamedThreads::GameThread,
			          []
			          {
				          ISourceControlModule::Get().GetProvider().Execute(ISourceControlOperation::Create<FUpdateStatus>(), {}, EConcurrency::Asynchronous);
			          });
		});

		return true;
	}

	return false;
}

TValueOrError<FString, FString> AnchorpointCliOperations::SubmitFiles(TArray<FString> InFiles, const FString& InMessage)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::SubmitFiles);

	TArray<FString> SubmitParams;
	SubmitParams.Add(TEXT("sync"));
	SubmitParams.Add(TEXT("--noProjectSaved"));
	SubmitParams.Add(TEXT("--files"));

	for (const FString& File : InFiles)
	{
		SubmitParams.Add(FString::Printf(TEXT("\"%s\""), *AnchorpointCliOperations::ConvertFullPathToApInternal(File)));
	}

	FString EscapedMessage = InMessage;
	EscapedMessage.ReplaceInline(TEXT("\r\n"), TEXT(R"(\\\r\\\n)"));
	SubmitParams.Add(FString::Printf(TEXT("--message \"%s\""), *EscapedMessage));

	FString SubmitCommand = FString::Join(SubmitParams, TEXT(" "));

	FCliParameters Parameters = {SubmitCommand};
	Parameters.OnProcessUpdate = IsSubmitFinished;

	FCliResult ProcessOutput = AnchorpointCliCommands::RunApCommand(Parameters);

	// NOTE: for the submit command, we can't rely on the DidSucceed() as the process will be deatched while LFS uploads,
	// Therefore no exit code will be available so we will check the output for errors instead.
	const FString PotentialError = ProcessOutput.GetBestError();
	if (!PotentialError.IsEmpty())
	{
		return MakeError(PotentialError);
	}

	return MakeValue(TEXT("Success"));
}

TValueOrError<FAnchorpointHistory, FString> AnchorpointCliOperations::GetHistoryInfo(const FString& InFile)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::GetHistoryInfo);

	TArray<FString> HistoryParams;
	HistoryParams.Add(TEXT("log"));
	HistoryParams.Add(TEXT("--file"));
	HistoryParams.Add(AnchorpointCliOperations::ConvertFullPathToApInternal(InFile));
	HistoryParams.Add(TEXT("--n"));
	HistoryParams.Add(TEXT("100")); //TODO: Talk to Jochen to remove this hardcoded thing and just get all the history

	FString HistoryCommand = FString::Join(HistoryParams, TEXT(" "));

	FCliResult ProcessOutput = AnchorpointCliCommands::RunApCommand(HistoryCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.GetBestError());
	}

	TArray<TSharedPtr<FJsonValue>> HistoryEntries = ProcessOutput.OutputAsJsonArray();
	TArray<FAnchorpointHistoryEntry> History = FAnchorpointHistoryEntry::ArrayFromJsonArray(HistoryEntries);

	return MakeValue(History);
}

TValueOrError<FAnchorpointConflictStatus, FString> AnchorpointCliOperations::GetConflictStatus(const FString& InFile)
{
	TArray<FString> ConflictParams;
	ConflictParams.Add(TEXT("ls-files"));
	ConflictParams.Add(TEXT("--unmerged"));

	//Note: This will run as a git command so we need project relative paths 
	ConflictParams.Add(ConvertFullPathToProjectRelative(InFile));

	FString ConflictCommand = FString::Join(ConflictParams, TEXT(" "));

	FCliResult ProcessOutput = AnchorpointCliCommands::RunGitCommand(ConflictCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.GetBestError());
	}

	TArray<FString> OutputLines;
	ProcessOutput.StdOutOutput.ParseIntoArray(OutputLines, TEXT("\n"), true);

	if (OutputLines.Num() != 3)
	{
		// Not output reported means no conflict
		FAnchorpointConflictStatus Result = {};
		return MakeValue(Result);
	}

	FAnchorpointConflictStatus Result = {OutputLines};
	Result.RemoteFilename = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / Result.RemoteFilename);
	Result.CommonAncestorFilename = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / Result.CommonAncestorFilename);

	return MakeValue(Result);
}


TValueOrError<FString, FString> AnchorpointCliOperations::DownloadFileViaCommitId(const FString& InCommitId, const FString& InFile, const FString& Destination)
{
	TArray<FString> RevisionParserParameters;
	RevisionParserParameters.Add(TEXT("rev-parse"));
	RevisionParserParameters.Add(FString::Printf(TEXT("%s:%s"), *InCommitId, *ConvertFullPathToApInternal(InFile)));

	const FString RevisionParserCommand = FString::Join(RevisionParserParameters, TEXT(" "));
	FCliResult ProcessOutput = AnchorpointCliCommands::RunGitCommand(RevisionParserCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.GetBestError());
	}

	const FString OID = ProcessOutput.StdOutOutput;
	if (OID.IsEmpty())
	{
		return MakeError(TEXT("No OID received"));
	}

	return DownloadFileViaOID(OID, InFile, Destination);
}

TValueOrError<FString, FString> AnchorpointCliOperations::DownloadFileViaOID(const FString& InOID, const FString& InFile, const FString& Destination)
{
	TArray<FString> DownloadParameters;
	DownloadParameters.Add(TEXT("cat-file"));
	DownloadParameters.Add(FString::Printf(TEXT("--filters %s"), *InOID));
	DownloadParameters.Add(FString::Printf(TEXT("--path %s"), *ConvertFullPathToApInternal(InFile)));

	FString DownloadCommand = FString::Join(DownloadParameters, TEXT(" "));

	FCliResult ProcessOutput = AnchorpointCliCommands::RunGitCommand(DownloadCommand, true);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.GetBestError());
	}

	TArray<uint8> BinaryResult = ProcessOutput.StdOutBinary;
	if (BinaryResult.IsEmpty())
	{
		return MakeError(TEXT("No data received"));
	}

	const bool bWriteSuccess = FFileHelper::SaveArrayToFile(BinaryResult, *Destination);
	if (!bWriteSuccess)
	{
		return MakeError(FString::Printf(TEXT("Failed to write file to %s"), *Destination));
	}

	return MakeValue(TEXT("Success"));
}

TValueOrError<FString, FString> AnchorpointCliOperations::MarkConflictSolved(const TArray<FString>& InFiles)
{
	FString ResolveCommand = TEXT("add");

	for (const FString& File : InFiles)
	{
		//Note: This will run as a git command so we need project relative paths 
		ResolveCommand.Appendf(TEXT(" '%s'"), *ConvertFullPathToProjectRelative(File));
	}

	FCliResult ProcessOutput = AnchorpointCliCommands::RunGitCommand(ResolveCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.GetBestError());
	}

	return MakeValue(TEXT("Success"));
}

void AnchorpointCliOperations::MonitorProcessWithNotification(const TSharedRef<FAnchorpointCliProcess>& Process, const FText& ProgressText, const FText& FinishText)
{
	struct FMonitorProcessNotification
	{
		FString Message;
		int Progress;
	};

	TSharedPtr<FMonitorProcessNotification> NotificationData = MakeShared<FMonitorProcessNotification>();
	NotificationData->Message = ProgressText.ToString();
	NotificationData->Progress = 0;

	AsyncTask(ENamedThreads::GameThread,
	          [=]()
	          {
		          FProgressNotificationHandle NotificationHandle = FSlateNotificationManager::Get().StartProgressNotification(ProgressText, 100);

		          // This process will be monitored in the background, so whatever was in the output doesn't matter anymore.
		          // So in order to simplify the parsing of the output, we clear it.
		          Process->ClearStdErrData();

		          Process->OnProcessUpdated.AddLambda([Process, NotificationData]()
		          {
			          FString StdErrString;
			          TArray<uint8> StdErrBinary;
			          Process->GetStdErrData(StdErrString, StdErrBinary);
			          Process->ClearStdErrData();

			          TSharedPtr<FJsonObject> JsonObject;
			          TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(StdErrString);
			          if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
			          {
				          return;
			          }

			          FString ProgressText;
			          JsonObject->TryGetStringField(TEXT("progress-text"), ProgressText);

			          int ProgressValue = 0;
			          JsonObject->TryGetNumberField(TEXT("progress-value"), ProgressValue);

			          NotificationData->Message = ProgressText;
			          NotificationData->Progress = ProgressValue;
		          });

		          FDelegateHandle PostTickHandle = FSlateApplication::Get().OnPostTick().AddLambda([NotificationHandle, NotificationData](float DeltaTime)
		          {
			          FSlateNotificationManager::Get().UpdateProgressNotification(NotificationHandle, NotificationData->Progress, 0, FText::FromString(NotificationData->Message));
		          });

		          Process->OnProcessEnded.AddLambda([NotificationHandle, PostTickHandle, FinishText]()
		          {
			          AsyncTask(ENamedThreads::GameThread,
			                    [NotificationHandle, PostTickHandle, FinishText]()
			                    {
				                    FSlateApplication::Get().OnPostTick().Remove(PostTickHandle);

				                    FSlateNotificationManager::Get().CancelProgressNotification(NotificationHandle);

				                    FNotificationInfo* Info = new FNotificationInfo(FinishText);
				                    Info->ExpireDuration = 5.0f;
				                    Info->Image = FAppStyle::Get().GetBrush("Icons.SuccessWithColor.Large");
				                    FSlateNotificationManager::Get().QueueNotification(Info);
			                    });
		          });
	          });
}
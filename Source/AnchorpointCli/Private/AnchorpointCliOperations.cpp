// Some copyright should be here...

#include "AnchorpointCliOperations.h"

#include <Misc/MonitoredProcess.h>

#include "AnchorpointCli.h"

DEFINE_LOG_CATEGORY_STATIC(LogAnchorpointCli, VeryVerbose, All);

FString ToRelativePath(const FString& InFullPath)
{
	FString RelativePath = InFullPath;
	const bool bSuccess = FPaths::MakePathRelativeTo(RelativePath, *FPaths::ProjectDir());

	UE_CLOG(!bSuccess, LogAnchorpointCli, Error, TEXT("Failed to make path relative: %s in directory: %s"), *InFullPath, *FPaths::ProjectDir());

	return RelativePath;
}

EAnchorpointFileOperation LexFromString(const FString& InString)
{
	if (InString == TEXT("A"))
	{
		return EAnchorpointFileOperation::Added;
	}
	if (InString == TEXT("M"))
	{
		return EAnchorpointFileOperation::Modified;
	}
	if (InString == TEXT("D"))
	{
		return EAnchorpointFileOperation::Deleted;
	}
	if (InString == TEXT("R"))
	{
		return EAnchorpointFileOperation::Renamed;
	}
	if (InString == TEXT("C"))
	{
		return EAnchorpointFileOperation::Conflicted;
	}

	return EAnchorpointFileOperation::Invalid;
}

FAnchorpointStatus FAnchorpointStatus::FromJson(const TSharedRef<FJsonObject>& InJsonObject)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointStatus::FromString);

	const FString ProjectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

	FAnchorpointStatus Result;
	Result.CurrentBranch = InJsonObject->GetStringField(TEXT("current_branch"));
	for (const TTuple<FString, TSharedPtr<FJsonValue>> StagedFile : InJsonObject->GetObjectField(TEXT("staged"))->Values)
	{
		FString FullFilePath = ProjectPath / StagedFile.Key;
		FPaths::NormalizeDirectoryName(FullFilePath);
		Result.Staged.Add(FullFilePath, LexFromString(StagedFile.Value->AsString()));
	}
	for (const TTuple<FString, TSharedPtr<FJsonValue>> NotStagedFile : InJsonObject->GetObjectField(TEXT("not_staged"))->Values)
	{
		FString FullFilePath = ProjectPath / NotStagedFile.Key;
		FPaths::NormalizeDirectoryName(FullFilePath);
		Result.NotStaged.Add(FullFilePath, LexFromString(NotStagedFile.Value->AsString()));
	}
	for (TTuple<FString, TSharedPtr<FJsonValue>> LockedFile : InJsonObject->GetObjectField(TEXT("locked_files"))->Values)
	{
		FString FullFilePath = ProjectPath / LockedFile.Key;
		FPaths::NormalizeDirectoryName(FullFilePath);
		Result.LockedFiles.Add(FullFilePath, LockedFile.Value->AsString());
	}
	for (const TSharedPtr<FJsonValue> OutDatedFile : InJsonObject->GetArrayField(TEXT("outdated_files")))
	{
		FString FullFilePath = ProjectPath / OutDatedFile->AsString();
		FPaths::NormalizeDirectoryName(FullFilePath);
		Result.OutdatedFiles.Add(FullFilePath);
	}

	return Result;
}

bool FCliResult::DidSucceed() const
{
	return !Error.IsSet();
}

TSharedPtr<FJsonObject> FCliResult::OutputAsJsonObject() const
{
	TSharedPtr<FJsonObject> JsonObject = nullptr;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Output);
	FJsonSerializer::Deserialize(Reader, JsonObject);

	return JsonObject;
}

TArray<TSharedPtr<FJsonValue>> FCliResult::OutputAsJsonArray() const
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Output);
	FJsonSerializer::Deserialize(Reader, JsonArray);

	return JsonArray;
}

bool AnchorpointCliOperations::IsInstalled()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::IsInstalled);

	const FString CliPath = GetCliPath();
	return !CliPath.IsEmpty() && FPaths::FileExists(CliPath);
}

TValueOrError<FString, FString> AnchorpointCliOperations::Connect()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::Connect);

	// TODO: Right now we are running a `status` command for checking connection,
	// but in the future we might want to use a dedicated command for connecting.
	// because of this, we want to make sure the command is both successful and the resulting output is valid (json object format).

	FCliResult ProcessOutput = RunApCommand(TEXT("status"));
	const bool bSuccessful = ProcessOutput.DidSucceed() && ProcessOutput.OutputAsJsonObject().IsValid();
	if (bSuccessful)
	{
		return MakeValue(TEXT("Success"));
	}

	if(!ProcessOutput.OutputAsJsonObject().IsValid())
	{
		return MakeError(TEXT("Failed to parse output to JSON."));
	}

	return MakeError(ProcessOutput.Error.GetValue());
}

TValueOrError<FString, FString> AnchorpointCliOperations::GetCurrentUser()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::GetCurrentUser);

	FCliResult ProcessOutput = RunApCommand(TEXT("user list"));
	if (ProcessOutput.DidSucceed())
	{
		TArray<TSharedPtr<FJsonValue>> Users = ProcessOutput.OutputAsJsonArray();
		for (const TSharedPtr<FJsonValue>& User : Users)
		{
			TSharedPtr<FJsonObject> UserObject = User->AsObject();
			if (UserObject->HasField(TEXT("current")) && UserObject->GetBoolField(TEXT("current")))
			{
				return MakeValue(UserObject->GetStringField(TEXT("email")));
			}
		}

		return MakeError(TEXT("Could not find current user"));
	}

	return MakeError(ProcessOutput.Error.GetValue());
}

TValueOrError<FAnchorpointStatus, FString> AnchorpointCliOperations::GetStatus()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::GetStatus);

	FCliResult ProcessOutput = RunApCommand(TEXT("status"));
	if (ProcessOutput.DidSucceed())
	{
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

		return MakeValue(FAnchorpointStatus::FromJson(Object.ToSharedRef()));
	}

	return MakeError(ProcessOutput.Error.GetValue());
}

TValueOrError<FString, FString> AnchorpointCliOperations::LockFiles(TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::LockFiles);

	TArray<FString> LockParams;
	LockParams.Add(TEXT("lock create"));
	LockParams.Add(TEXT("--git"));
	LockParams.Add(TEXT("--keep"));
	LockParams.Add(TEXT("--files"));

	for (const FString& File : InFiles)
	{
		LockParams.Add(FString::Printf(TEXT("\"%s\""), *ToRelativePath(File)));
	}

	FString LockCommand = FString::Join(LockParams, TEXT(" "));
	FCliResult ProcessOutput = RunApCommand(LockCommand);

	if (ProcessOutput.DidSucceed())
	{
		return MakeValue(TEXT("Success"));
	}

	return MakeError(ProcessOutput.Error.GetValue());
}

TValueOrError<FString, FString> AnchorpointCliOperations::UnlockFiles(TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::UnlockFiles);

	TArray<FString> UnlockParams;
	UnlockParams.Add(TEXT("lock remove"));
	UnlockParams.Add(TEXT("--files"));

	for (const FString& File : InFiles)
	{
		UnlockParams.Add(FString::Printf(TEXT("\"%s\""), *ToRelativePath(File)));
	}

	FString LockCommand = FString::Join(UnlockParams, TEXT(" "));
	FCliResult ProcessOutput = RunApCommand(LockCommand);

	if (ProcessOutput.DidSucceed())
	{
		return MakeValue(TEXT("Success"));
	}

	return MakeError(ProcessOutput.Error.GetValue());
}

TValueOrError<FString, FString> AnchorpointCliOperations::DiscardChanges(TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::DiscardChanges);

	//TODO: Check how revert only unchanged check affects this

	FString CheckoutCommand = TEXT("checkout --");

	for (const FString& File : InFiles)
	{
		CheckoutCommand.Appendf(TEXT(" '%s'"), *ToRelativePath(File));
	}

	FCliResult ProcessOutput = RunGitCommand(CheckoutCommand);

	if (ProcessOutput.DidSucceed())
	{
		return MakeValue(TEXT("Success"));
	}

	return MakeError(ProcessOutput.Error.GetValue());
}

FString AnchorpointCliOperations::GetCliPath()
{
	const FString CliDirectory = FAnchorpointCliModule::Get().GetCliPath();

#if PLATFORM_WINDOWS
	return CliDirectory / "ap.exe";
#elif PLATFORM_MAC
	return CliDirectory / "ap";
#endif
}

#define WAIT_FOR_CONDITION(x) while(x) continue;

FCliResult AnchorpointCliOperations::RunApCommand(const FString& InCommand, bool bRequestJsonOutput)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(RunApCli);

	FCliResult Result;
	if (!IsInstalled())
	{
		Result.Error = TEXT("Anchorpoint not installed at the current path");
		return Result;
	}

	TSharedPtr<FMonitoredProcess> Process = nullptr;

	FString CommandLineExecutable = GetCliPath();

	TArray<FString> Args;
	Args.Add(FString::Printf(TEXT("--cwd=\"%s\""), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));

	if (bRequestJsonOutput)
	{
		Args.Add(TEXT("--json"));
	}

	Args.Add(TEXT("--apiVersion 1"));
	Args.Add(InCommand);

	const FString CommandLineArgs = FString::Join(Args, TEXT(" "));
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Running %s %s"), *CommandLineExecutable, *CommandLineArgs);
	Process = MakeShared<FMonitoredProcess>(CommandLineExecutable, CommandLineArgs, true);

	if (!Process)
	{
		Result.Error = TEXT("Failed to create process");
		return Result;
	}

	const bool bLaunchSuccess = Process->Launch();
	if (!bLaunchSuccess)
	{
		Result.Error = TEXT("Failed to launch process");
		return Result;
	}

	WAIT_FOR_CONDITION(Process->Update());

	if (Process->GetReturnCode() != 0)
	{
		Result.Error = FString::Printf(TEXT("Process failed with code %d"), Process->GetReturnCode());
		return Result;
	}

	// TODO: We should bind to this delegate all discard all the waiting for daemon message
	Result.Output = Process->GetFullOutputWithoutDelegate();
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("CLI output: %s"), *Result.Output);

	return Result;
}

FCliResult AnchorpointCliOperations::RunGitCommand(const FString& InCommand)
{
	const FString GitViaAp = FString::Printf(TEXT("git --command \"%s\""), *InCommand);
	return RunApCommand(GitViaAp, false);
}
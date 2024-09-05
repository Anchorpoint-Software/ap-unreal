// Some copyright should be here...

#include "AnchorpointCliOperations.h"

#include <Misc/MonitoredProcess.h>

#include "AnchorpointCli.h"

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

	// TODO: Talk with AP team - staged & unstaged are relative, but the locked files are full path
	FAnchorpointStatus Result;
	Result.CurrentBranch = InJsonObject->GetStringField(TEXT("current_branch"));
	for (const TTuple<FString, TSharedPtr<FJsonValue>> StagedFile : InJsonObject->GetObjectField(TEXT("staged"))->Values)
	{
		Result.Staged.Add(ProjectPath / StagedFile.Key, LexFromString(StagedFile.Value->AsString()));
	}
	for (const TTuple<FString, TSharedPtr<FJsonValue>> NotStagedFile : InJsonObject->GetObjectField(TEXT("not_staged"))->Values)
	{
		Result.NotStaged.Add(ProjectPath / NotStagedFile.Key, LexFromString(NotStagedFile.Value->AsString()));
	}
	for (TTuple<FString, TSharedPtr<FJsonValue>> LockedFile : InJsonObject->GetObjectField(TEXT("locked_files"))->Values)
	{
		Result.LockedFiles.Add(ProjectPath / LockedFile.Key, LockedFile.Value->AsString());
	}
	for (const TSharedPtr<FJsonValue> OutDatedFile : InJsonObject->GetArrayField(TEXT("outdated_files")))
	{
		Result.OutdatedFiles.Add(ProjectPath / OutDatedFile->AsString());
	}

	return Result;
}

bool FCliOutput::DidSucceed() const
{
	return !Error.IsSet();
}

TSharedPtr<FJsonObject> FCliOutput::OutputAsJsonObject() const
{
	TSharedPtr<FJsonObject> JsonObject = nullptr;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Output);
	FJsonSerializer::Deserialize(Reader, JsonObject);

	return JsonObject;
}

TArray<TSharedPtr<FJsonValue>> FCliOutput::OutputAsJsonArray() const
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Output);
	FJsonSerializer::Deserialize(Reader, JsonArray);

	return JsonArray;
}

TValueOrError<FString, FString> AnchorpointCliOperations::Connect()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::Connect);

	// TODO: Right now we are running a `status` command for checking connection,
	// but in the future we might want to use a dedicated command for connecting.

	FCliOutput ProcessOutput = RunApCommand(TEXT("status"));
	if (ProcessOutput.DidSucceed())
	{
		return MakeValue(TEXT("Success"));
	}

	return MakeError(ProcessOutput.Error.GetValue());
}

TValueOrError<FString, FString> AnchorpointCliOperations::GetCurrentUser()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::GetCurrentUser);

	FCliOutput ProcessOutput = RunApCommand(TEXT("user list"));
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

	FCliOutput ProcessOutput = RunApCommand(TEXT("status"));
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
		LockParams.Add(FString::Printf(TEXT("\"%s\""), *File));
	}

	FString LockCommand = FString::Join(LockParams, TEXT(" "));
	FCliOutput ProcessOutput = RunApCommand(LockCommand);

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
		UnlockParams.Add(FString::Printf(TEXT("\"%s\""), *File));
	}

	FString LockCommand = FString::Join(UnlockParams, TEXT(" "));
	FCliOutput ProcessOutput = RunApCommand(LockCommand);

	if (ProcessOutput.DidSucceed())
	{
		return MakeValue(TEXT("Success"));
	}

	return MakeError(ProcessOutput.Error.GetValue());
}

TValueOrError<FString, FString> AnchorpointCliOperations::DiscardChanges(TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::DiscardChanges);

	FString CheckoutCommand = TEXT("checkout");

	for (const FString& File : InFiles)
	{
		//TODO: Talk with AP team how we can add quotes around file name in case they have spaces
		CheckoutCommand.Appendf(TEXT(" %s"), *File);
	}

	FCliOutput ProcessOutput = RunGitCommand(CheckoutCommand);

	if (ProcessOutput.DidSucceed())
	{
		return MakeValue(TEXT("Success"));
	}

	return MakeError(ProcessOutput.Error.GetValue());
}

#define WAIT_FOR_CONDITION(x) while(x) continue;

FCliOutput AnchorpointCliOperations::RunApCommand(const FString& InCommand, bool bRequestJsonOutput)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(RunApCli);

	TSharedPtr<FMonitoredProcess> Process = nullptr;

	{
		const FString CliDirectory = FAnchorpointCliModule::Get().GetCliPath();

#if PLATFORM_WINDOWS
		const FString CommandLineExecutable = CliDirectory / "ap.exe";
#elif PLATFORM_MAC
		const FString CommandLineExecutable = CliDirectory / "ap";
#endif

		TArray<FString> Args;
		Args.Add(FString::Printf(TEXT("--cwd=\"%s\""), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));

		if (bRequestJsonOutput)
		{
			Args.Add(TEXT("--json"));
		}

		Args.Add(TEXT("--apiVersion 1"));

		Args.Add(InCommand);

		FString CommandLineArgs = FString::Join(Args, TEXT(" "));
		Process = MakeShared<FMonitoredProcess>(CommandLineExecutable, CommandLineArgs, true);
	}

	FCliOutput Output;
	if (!Process)
	{
		Output.Error = TEXT("Failed to create process");
		return Output;
	}

	const bool bLaunchSuccess = Process->Launch();
	if (!bLaunchSuccess)
	{
		Output.Error = TEXT("Failed to launch process");
		return Output;
	}

	WAIT_FOR_CONDITION(Process->Update());

	if (Process->GetReturnCode() != 0)
	{
		Output.Error = FString::Printf(TEXT("Process failed with code %d"), Process->GetReturnCode());
		return Output;
	}

	Output.Output = Process->GetFullOutputWithoutDelegate();

	return Output;
}

FCliOutput AnchorpointCliOperations::RunGitCommand(const FString& InCommand)
{
	const FString GitViaAp = FString::Printf(TEXT("git --command \"%s\""), *InCommand);
	return RunApCommand(GitViaAp, false);
}
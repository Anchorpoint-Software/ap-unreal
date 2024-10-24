#include "AnchorpointCliOperations.h"

#include <Misc/MonitoredProcess.h>

#include "AnchorpointCli.h"
#include "AnchorpointCliLog.h"

FString ToRelativePath(const FString& InFullPath)
{
	FString RelativePath = InFullPath;
	const bool bSuccess = FPaths::MakePathRelativeTo(RelativePath, *FPaths::ProjectDir());

	UE_CLOG(!bSuccess, LogAnchorpointCli, Error, TEXT("Failed to make path relative: %s in directory: %s"), *InFullPath, *FPaths::ProjectDir());

	return RelativePath;
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

	const FString CliPath = FAnchorpointCliModule::Get().GetCliPath();
	return !CliPath.IsEmpty() && FPaths::FileExists(CliPath);
}

void AnchorpointCliOperations::ShowInAnchorpoint(const FString& InPath)
{
	FPlatformProcess::CreateProc(*FAnchorpointCliModule::Get().GetApplicationPath(), *InPath, true, true, false, nullptr, 0, nullptr, nullptr);
}

TValueOrError<FString, FString> AnchorpointCliOperations::GetCurrentUser()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::GetCurrentUser);

	static FString CurrentUser;

	if (!CurrentUser.IsEmpty())
	{
		return MakeValue(CurrentUser);
	}

	FCliResult ProcessOutput = RunApCommand(TEXT("user list"));
	if (ProcessOutput.DidSucceed())
	{
		TArray<TSharedPtr<FJsonValue>> Users = ProcessOutput.OutputAsJsonArray();
		for (const TSharedPtr<FJsonValue>& User : Users)
		{
			TSharedPtr<FJsonObject> UserObject = User->AsObject();
			if (UserObject->HasField(TEXT("current")) && UserObject->GetBoolField(TEXT("current")))
			{
				CurrentUser = UserObject->GetStringField(TEXT("email"));
				return MakeValue(CurrentUser);
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

TValueOrError<FString, FString> AnchorpointCliOperations::Revert(TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::Revert);

	TArray<FString> RevertParams;
	RevertParams.Add(TEXT("revert"));
	RevertParams.Add(TEXT("--files"));

	for (const FString& File : InFiles)
	{
		RevertParams.Add(FString::Printf(TEXT("\"%s\""), *ToRelativePath(File)));
	}

	FString RevertCommand = FString::Join(RevertParams, TEXT(" "));
	FCliResult ProcessOutput = RunApCommand(RevertCommand);

	if (ProcessOutput.DidSucceed())
	{
		return MakeValue(TEXT("Success"));
	}

	return MakeError(ProcessOutput.Error.GetValue());
}

TValueOrError<FString, FString> AnchorpointCliOperations::DeleteFiles(TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::DeleteFiles);

	FString CheckoutCommand = TEXT("rm --");

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

TValueOrError<FString, FString> AnchorpointCliOperations::SubmitFiles(TArray<FString> InFiles, const FString& InMessage)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::SubmitFiles);

	TArray<FString> SubmitParams;
	SubmitParams.Add(TEXT("sync"));
	SubmitParams.Add(TEXT("--files"));

	for (const FString& File : InFiles)
	{
		SubmitParams.Add(FString::Printf(TEXT("\"%s\""), *ToRelativePath(File)));
	}

	SubmitParams.Add(FString::Printf(TEXT("--message \"%s\""), *InMessage));

	FString SubmitCommand = FString::Join(SubmitParams, TEXT(" "));
	FCliResult ProcessOutput = RunApCommand(SubmitCommand);

	if (ProcessOutput.DidSucceed())
	{
		return MakeValue(TEXT("Success"));
	}

	return MakeError(ProcessOutput.Error.GetValue());
}

FString AnchorpointCliOperations::ConvertCommandToIni(const FString& InCommand, bool bPrintConfig /*= false*/)
{
	TArray<FString> Result;

	// config printing is only used by automated tests
	if (bPrintConfig)
	{
		Result.Add(TEXT("print-config=true"));
	}

	// Add the default parameters
	Result.Add(FString::Printf(TEXT("cwd=\"%s\""), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));
	Result.Add(TEXT("json=true"));
	Result.Add(TEXT("apiVersion=1"));

	FString CurrentKey;
	TArray<FString> CurrentCommands;
	TMap<FString, TArray<FString>> ValuesMap;
	const TCHAR* CommandLine = *InCommand;

	FString Token;
	while (FParse::Token(CommandLine, Token, true))
	{
		if (Token.RemoveFromStart(TEXT("--")))
		{
			CurrentKey = Token;
			ValuesMap.Add(CurrentKey, {});
		}
		else if (CurrentKey.IsEmpty())
		{
			CurrentCommands.Add(Token);
			Result.Add(FString::Printf(TEXT("[%s]"), *FString::Join(CurrentCommands, TEXT("."))));
		}
		else
		{
			ValuesMap[CurrentKey].Add(FString::Printf(TEXT("\"%s\""), *Token));;
		}
	}

	for (const TTuple<FString, TArray<FString>>& ValuePair : ValuesMap)
	{
		FString Line = FString::Printf(TEXT("%s="), *ValuePair.Key);

		if(ValuePair.Value.IsEmpty())
		{
			Line.Appendf(TEXT("true"));
		}
		else if (ValuePair.Value.Num() == 1)
		{
			Line.Appendf(TEXT("%s"), *ValuePair.Value[0]);
		}
		else if (ValuePair.Value.Num() > 1)
		{
			Line.Appendf(TEXT("[%s]"), *FString::Join(ValuePair.Value, TEXT(", ")));
		}

		Result.Add(Line);
	}

	Result.Add(LINE_TERMINATOR);
	return FString::Join(Result, LINE_TERMINATOR);
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

	FString CommandLineExecutable = FAnchorpointCliModule::Get().GetCliPath();

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
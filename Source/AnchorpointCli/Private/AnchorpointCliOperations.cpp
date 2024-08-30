// Some copyright should be here...

#include "AnchorpointCliOperations.h"

#include <Misc/MonitoredProcess.h>
#include <JsonObjectWrapper.h>

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
	if(InString == TEXT("R"))
	{
		return EAnchorpointFileOperation::Renamed;
	}
	if (InString == TEXT("C"))
	{
		return EAnchorpointFileOperation::Conflicted;
	}

	return EAnchorpointFileOperation::Invalid;
}

FAnchorpointStatus FAnchorpointStatus::FromString(const FString& InString)
{
	FJsonObjectWrapper JsonObject;
	JsonObject.JsonObjectFromString(InString);

	FString ProjectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

	// TODO: Talk with AP team - staged & unstaged are relative, but the locked files are full path
	FAnchorpointStatus Result;
	Result.CurrentBranch = JsonObject.JsonObject->GetStringField(TEXT("current_branch"));
	for (const TTuple<FString, TSharedPtr<FJsonValue>> StagedFile : JsonObject.JsonObject->GetObjectField(TEXT("staged"))->Values)
	{
		Result.Staged.Add(ProjectPath / StagedFile.Key, LexFromString(StagedFile.Value->AsString()));
	}
	for (const TTuple<FString, TSharedPtr<FJsonValue>> NotStagedFile : JsonObject.JsonObject->GetObjectField(TEXT("not_staged"))->Values)
	{
		Result.NotStaged.Add(ProjectPath / NotStagedFile.Key, LexFromString(NotStagedFile.Value->AsString()));
	}
	for (TTuple<FString, TSharedPtr<FJsonValue>> LockedFile : JsonObject.JsonObject->GetObjectField(TEXT("locked_files"))->Values)
	{
		Result.LockedFiles.Add(LockedFile.Key, LockedFile.Value->AsString());
	}
	for (const TSharedPtr<FJsonValue> OutDatedFile : JsonObject.JsonObject->GetArrayField(TEXT("outdated_files")))
	{
		Result.OutdatedFiles.Add(OutDatedFile->AsString());
	}

	return Result;
}

TSharedPtr<FMonitoredProcess> RunApCli(const FString& InCommand)
{
	const FString CliDirectory = FAnchorpointCliModule::Get().GetCliPath();

#if PLATFORM_WINDOWS
	const FString CommandLineExecutable = CliDirectory / "ap.exe";
#elif PLATFORM_MAC
	const FString CommandLineExecutable = CliDirectory / "ap";
#endif

	TArray<FString> Args;
	Args.Add(FString::Printf(TEXT("--cwd=%s"), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));
	Args.Add(TEXT("--json"));
	Args.Add(TEXT("--apiVersion 1"));

	Args.Add(InCommand);

	FString CommandLineArgs = FString::Join(Args, TEXT(" "));
	TSharedRef<FMonitoredProcess> Process = MakeShared<FMonitoredProcess>(CommandLineExecutable, CommandLineArgs, true);

	const bool bLaunchSuccess = Process->Launch();
	if (!bLaunchSuccess)
	{
		return nullptr;
	}

	while (Process->Update())
	{
	}

	return Process;
}

TSharedPtr<FJsonObject> JsonStringToJsonObject(const FString& InJsonString)
{
	TSharedPtr<FJsonObject> JsonObject = nullptr;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(InJsonString);
	FJsonSerializer::Deserialize(Reader, JsonObject);

	return JsonObject;
}

FString GetNiceOutput(const FString& InOutput)
{
	FString Result = InOutput;
	int StartIndex = INDEX_NONE;
	Result.FindChar('{', StartIndex);
	int EndIndex = INDEX_NONE;
	Result.FindLastChar('}', EndIndex);
	Result.MidInline(StartIndex, EndIndex - StartIndex + 1);

	return Result;
}

TValueOrError<FString, FString> AnchorpointCliOperations::Connect()
{
	// TODO: Right now we are running a `status` command for checking connection,
	// but in the future we might want to use a dedicated command for it.

	TSharedPtr<FMonitoredProcess> Process = RunApCli(TEXT("status"));
	if (!Process || Process->GetReturnCode() != 0)
	{
		return MakeError(TEXT("Failed to run CLI"));
	}


	const FString Output = GetNiceOutput(Process->GetFullOutputWithoutDelegate());
	TSharedPtr<FJsonObject> Object = JsonStringToJsonObject(Output);

	if (!Object.IsValid())
	{
		return MakeError(FString::Printf(TEXT("Failed to parse output: %s"), *Output));
	}

	FString Error;
	if (Object->TryGetStringField(TEXT("error"), Error))
	{
		return MakeError(FString::Printf(TEXT("CLI error: %s"), *Output));
	}

	return MakeValue(TEXT("Success"));
}

TValueOrError<FString, FString> AnchorpointCliOperations::GetCurrentUser()
{
	return MakeValue(TEXT("alexandru@outofthebox-plugins.com"));
}

TValueOrError<FAnchorpointStatus, FString> AnchorpointCliOperations::GetStatus()
{
	TSharedPtr<FMonitoredProcess> Process = RunApCli(TEXT("status"));
	if (!Process || Process->GetReturnCode() != 0)
	{
		return MakeError(TEXT("Failed to run CLI"));
	}

	const FString Output = GetNiceOutput(Process->GetFullOutputWithoutDelegate());
	TSharedPtr<FJsonObject> Object = JsonStringToJsonObject(Output);

	if (!Object.IsValid())
	{
		return MakeError(FString::Printf(TEXT("Failed to parse output: %s"), *Output));
	}

	FString Error;
	if (Object->TryGetStringField(TEXT("error"), Error))
	{
		return MakeError(FString::Printf(TEXT("CLI error: %s"), *Output));
	}

	return MakeValue(FAnchorpointStatus::FromString(Output));
}

TValueOrError<FString, FString> AnchorpointCliOperations::LockFiles(TArray<FString>& InFiles)
{
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
	TSharedPtr<FMonitoredProcess> Process = RunApCli(LockCommand);

	if (!Process || Process->GetReturnCode() != 0)
	{
		return MakeError(TEXT("Failed to run CLI"));
	}

	const FString Output = GetNiceOutput(Process->GetFullOutputWithoutDelegate());

	//TODO: Check with AP team - for some reaosn this returns an empty response, is that intended?
	if (Output.IsEmpty())
	{
		return MakeValue(TEXT("Success"));
	}

	TSharedPtr<FJsonObject> Object = JsonStringToJsonObject(Output);

	if (!Object.IsValid())
	{
		return MakeError(FString::Printf(TEXT("Failed to parse output: %s"), *Output));
	}

	FString Error;
	if (Object->TryGetStringField(TEXT("error"), Error))
	{
		return MakeError(FString::Printf(TEXT("CLI error: %s"), *Output));
	}

	return MakeValue(TEXT("Success"));
}
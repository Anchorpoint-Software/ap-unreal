#include "AnchorpointCliOperations.h"

#include "AnchorpointCli.h"
#include "AnchorpointCliLog.h"
#include "AnchorpointCliUtils.h"

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
	if(!RepositoryRootPath.IsEmpty())
	{
		return RepositoryRootPath;
	}

	// Searching for the .approject file in the project directory and all its parent directories
	FString SearchPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

	while (!SearchPath.IsEmpty())
	{
		TArray<FString> AnchorpointProjectFile;
		IFileManager::Get().FindFiles(AnchorpointProjectFile, *(SearchPath / TEXT("*.approj")), true, false);

		if(!AnchorpointProjectFile.IsEmpty())
		{
			RepositoryRootPath = SearchPath;
			return RepositoryRootPath;
		}

		SearchPath = FPaths::GetPath(SearchPath);
	}

	return {};
}

FString AnchorpointCliOperations::ConvertFullPathToApInternal(const FString& InFullPath)
{
	FString Path = InFullPath;
	FPaths::CollapseRelativeDirectories(Path);

	const FString RootFolder = GetRepositoryRootPath() + TEXT("/");
	Path = Path.Replace(*RootFolder, TEXT(""), ESearchCase::CaseSensitive);

	return Path;
}

FString AnchorpointCliOperations::ConvertApInternalToFull(const FString& InRelativePath)
{
	FString Path = GetRepositoryRootPath() / InRelativePath;
	FPaths::NormalizeDirectoryName(Path);

	return Path;
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

	FCliResult ProcessOutput = AnchorpointCliUtils::RunApCommand(TEXT("user list"));
	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.Error.GetValue());
	}

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

TValueOrError<FAnchorpointStatus, FString> AnchorpointCliOperations::GetStatus(const TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::GetStatus);

	TArray<FString> StatusParams;
	StatusParams.Add(TEXT("status"));

	if(!StatusParams.IsEmpty())
	{
		StatusParams.Add(TEXT("--paths"));
		for (const FString& File : InFiles)
		{
			StatusParams.Add(FString::Printf(TEXT("\"%s\""), *AnchorpointCliOperations::ConvertFullPathToApInternal(File)));
		}
	}

	FString StatusCommand = FString::Join(StatusParams, TEXT(" "));
	FCliResult ProcessOutput = AnchorpointCliUtils::RunApCommand(StatusCommand);
	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.Error.GetValue());
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

	return MakeValue(FAnchorpointStatus::FromJson(Object.ToSharedRef()));
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
	FCliResult ProcessOutput = AnchorpointCliUtils::RunApCommand(LockCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.Error.GetValue());
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
	FCliResult ProcessOutput = AnchorpointCliUtils::RunApCommand(UnlockCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.Error.GetValue());
	}

	return MakeValue(TEXT("Success"));
}

TValueOrError<FString, FString> AnchorpointCliOperations::Revert(const TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::Revert);

	TArray<FString> RevertParams;
	RevertParams.Add(TEXT("revert"));
	RevertParams.Add(TEXT("--files"));

	for (const FString& File : InFiles)
	{
		RevertParams.Add(FString::Printf(TEXT("\"%s\""), *AnchorpointCliOperations::ConvertFullPathToApInternal(File)));
	}

	FString RevertCommand = FString::Join(RevertParams, TEXT(" "));
	FCliResult ProcessOutput = AnchorpointCliUtils::RunApCommand(RevertCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.Error.GetValue());
	}

	return MakeValue(TEXT("Success"));
}

TValueOrError<FString, FString> AnchorpointCliOperations::DeleteFiles(const TArray<FString>& InFiles)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::DeleteFiles);

	FString CheckoutCommand = TEXT("rm --");

	for (const FString& File : InFiles)
	{
		CheckoutCommand.Appendf(TEXT(" '%s'"), *AnchorpointCliOperations::ConvertFullPathToApInternal(File));
	}

	FCliResult ProcessOutput = AnchorpointCliUtils::RunGitCommand(CheckoutCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.Error.GetValue());
	}

	return MakeValue(TEXT("Success"));
}

TValueOrError<FString, FString> AnchorpointCliOperations::SubmitFiles(TArray<FString> InFiles, const FString& InMessage)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliOperations::SubmitFiles);

	TArray<FString> SubmitParams;
	SubmitParams.Add(TEXT("sync"));
	SubmitParams.Add(TEXT("--files"));

	for (const FString& File : InFiles)
	{
		SubmitParams.Add(FString::Printf(TEXT("\"%s\""), *AnchorpointCliOperations::ConvertFullPathToApInternal(File)));
	}

	SubmitParams.Add(FString::Printf(TEXT("--message \"%s\""), *InMessage));

	FString SubmitCommand = FString::Join(SubmitParams, TEXT(" "));
	FCliResult ProcessOutput = AnchorpointCliUtils::RunApCommand(SubmitCommand);

	if (!ProcessOutput.DidSucceed())
	{
		return MakeError(ProcessOutput.Error.GetValue());
	}

	return MakeValue(TEXT("Success"));
}

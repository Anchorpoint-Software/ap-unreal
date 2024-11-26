#include "AnchorpointCliUtils.h"

#include <Misc/MonitoredProcess.h>

#include "AnchorpointCli.h"
#include "AnchorpointCliLog.h"

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

FCliParameters::FCliParameters(const FString& InCommand)
	: Command(InCommand)
	  , bRequestJsonOutput(true)
	  , OnProcessUpdate(nullptr)
{
}

FString AnchorpointCliUtils::ConvertCommandToIni(const FString& InCommand, bool bPrintConfig /* = false */, bool bJsonOutput /* = true */)
{
	TArray<FString> Result;

	// config printing is only used by automated tests
	if (bPrintConfig)
	{
		Result.Add(TEXT("print-config=true"));
	}

	// Add the default parameters
	Result.Add(FString::Printf(TEXT("cwd=\"%s\""), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));

	if (bJsonOutput)
	{
		Result.Add(TEXT("json=true"));
	}

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

	// Order doesn't matter but to match the output of --print-config, we sort them alphabeticaly 
	ValuesMap.KeySort(TLess<FString>());

	for (const TTuple<FString, TArray<FString>>& ValuePair : ValuesMap)
	{
		FString Line = FString::Printf(TEXT("%s="), *ValuePair.Key);

		if (ValuePair.Value.IsEmpty())
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

FCliResult AnchorpointCliUtils::RunApCommand(const FString& InCommand)
{
	FCliParameters Parameters = {InCommand};
	return RunApCommand(Parameters);
}

FCliResult AnchorpointCliUtils::RunApCommand(const FCliParameters& InParameters)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(RunApCli);

	FCliResult Result;

	FString IniConfigFile = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("ap-command.ini"));
	FString IniConfigContent = AnchorpointCliUtils::ConvertCommandToIni(InParameters.Command, false, InParameters.bRequestJsonOutput);
	FFileHelper::SaveStringToFile(IniConfigContent, *(IniConfigFile));

	TSharedPtr<FMonitoredProcess> Process = nullptr;

	FString CommandLineExecutable = FAnchorpointCliModule::Get().GetCliPath();

	TArray<FString> Args;

	Args.Add(FString::Printf(TEXT("--config=\"%s\""), *IniConfigFile));

	const FString CommandLineArgs = FString::Join(Args, TEXT(" "));
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Running %s %s"), *CommandLineExecutable, *CommandLineArgs);
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Ini content: %s"), *IniConfigContent);
	Process = MakeShared<FMonitoredProcess>(CommandLineExecutable, CommandLineArgs, true);

	if (!Process)
	{
		Result.Error = TEXT("Failed to create process");
		return Result;
	}

	FString ProcessOutput;
	FCriticalSection ProcessOutputLock;
	
	Process->OnOutput().BindLambda([&ProcessOutput, &ProcessOutputLock](const FString& NewOutput)
	{
		FScopeLock _(&ProcessOutputLock);
		if (NewOutput.Contains(TEXT("Waiting for Anchorpoint Daemon")))
		{
			UE_LOG(LogAnchorpointCli, Warning, TEXT("Discarding daemon message: %s"), *NewOutput);
			return;
		}

		ProcessOutput.Append(NewOutput);
	});

	const bool bLaunchSuccess = Process->Launch();
	if (!bLaunchSuccess)
	{
		Result.Error = TEXT("Failed to launch process");
		return Result;
	}

	bool bProcessIsRunning = true;
	bool bEarlyOutRequested = false;
	while (bProcessIsRunning && !bEarlyOutRequested)
	{
		bProcessIsRunning = Process->Update();

		FScopeLock _(&ProcessOutputLock);
		bEarlyOutRequested = InParameters.OnProcessUpdate && InParameters.OnProcessUpdate(Process, ProcessOutput);
	}

	if (Process->GetReturnCode() != 0)
	{
		Result.Error = FString::Printf(TEXT("Process failed with code %d"), Process->GetReturnCode());
		return Result;
	}

	// We need to perform this final append to ensure we get every bit of output after the program has finished
	ProcessOutput.Append(Process->GetFullOutputWithoutDelegate());

	Result.Output = ProcessOutput;
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("CLI output: %s"), *Result.Output);

	if (bProcessIsRunning)
	{
		// If the process is still running, it means we exited early due to the OnProcessUpdate callback, in this case:

		// 1. We will stop monitoring the output to prevent any memory access violations
		Process->OnOutput().Unbind();

		// 2. Let the process run to completion on a separated thread
		AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask,
		          [Process]()
		          {
			          while (Process && Process->Update()) { continue; }
		          });
	}

	return Result;
}

FCliResult AnchorpointCliUtils::RunGitCommand(const FString& InCommand)
{
	FCliParameters Parameters = {FString::Printf(TEXT("git --command \"%s\""), *InCommand)};
	Parameters.bRequestJsonOutput = false;

	return RunApCommand(Parameters);
}
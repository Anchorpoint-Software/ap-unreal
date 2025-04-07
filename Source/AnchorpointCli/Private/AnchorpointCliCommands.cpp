// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointCliCommands.h"

#include <Serialization/JsonSerializer.h>
#include <Async/Async.h>

#include "AnchorpointCliLog.h"
#include "AnchorpointCliProcess.h"

bool FCliResult::DidSucceed() const
{
	return ReturnCode.IsSet() && ReturnCode.GetValue() == 0;
}

TSharedPtr<FJsonObject> FCliResult::OutputAsJsonObject() const
{
	TSharedPtr<FJsonObject> JsonObject = nullptr;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(StdErrOutput);
	FJsonSerializer::Deserialize(Reader, JsonObject);

	return JsonObject;
}

TArray<TSharedPtr<FJsonValue>> FCliResult::OutputAsJsonArray() const
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(StdErrOutput);
	FJsonSerializer::Deserialize(Reader, JsonArray);

	return JsonArray;
}

FString FCliResult::GetBestError() const
{
	TSharedPtr<FJsonObject> JsonOutput = OutputAsJsonObject();

	if (JsonOutput.IsValid())
	{
		FString LastError;
		bool bHasErrorField = JsonOutput->TryGetStringField(TEXT("error"), LastError);

		if (bHasErrorField)
		{
			return LastError;
		}
	}

	if (ReturnCode.IsSet())
	{
		return FString::Printf(TEXT("Process failed with code %d"), ReturnCode.GetValue());
	}

	return {};
}

FCliParameters::FCliParameters(const FString& InCommand)
	: Command(InCommand)
{
}

FString AnchorpointCliCommands::ConvertCommandToIni(const FString& InCommand, bool bPrintConfig /* = false */, bool bJsonOutput /* = true */)
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

	return FString::Join(Result, LINE_TERMINATOR);
}

FCliResult AnchorpointCliCommands::RunApCommand(const FCliParameters& InParameters)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(RunApCli);

	TSharedRef<FAnchorpointCliProcess> Process = MakeShared<FAnchorpointCliProcess>();

	Process->Launch(InParameters);

	bool bProcessIsRunning = true;
	bool bEarlyOutRequested = false;
	while (bProcessIsRunning && !bEarlyOutRequested)
	{
		bProcessIsRunning = Process->IsRunning();

		FString StdErrString;
		TArray<uint8> StdErrBinary;
		Process->GetStdErrData(StdErrString, StdErrBinary);

		bEarlyOutRequested = InParameters.OnProcessUpdate && InParameters.OnProcessUpdate(Process, StdErrString);
	}

	if (bProcessIsRunning)
	{
		// If the process is still running, it means we exited early due to the OnProcessUpdate callback
		// So we let the process run to completion on a separated thread
		AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask,
		          [Process]()
		          {
			          while (Process->IsRunning()) { continue; }
		          });
	}

	FCliResult Result = Process->GetResult();
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("CLI stdout output: %s"), *Result.StdOutOutput);
	UE_LOG(LogAnchorpointCli, Verbose, TEXT("CLI stderr output: %s"), *Result.StdErrOutput);

	return Result;
}

FCliResult AnchorpointCliCommands::RunGitCommand(const FString& InCommand, bool bUseBinaryData /* = false */)
{
	FCliParameters Parameters = {FString::Printf(TEXT("git --command \"%s\""), *InCommand)};
	Parameters.bRequestJsonOutput = false;
	Parameters.bUseBinaryOutput = bUseBinaryData;

	return RunApCommand(Parameters);
}
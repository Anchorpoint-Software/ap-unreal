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

FString AnchorpointCliUtils::ConvertCommandToIni(const FString& InCommand, bool bPrintConfig /* = false */)
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


#define WAIT_FOR_CONDITION(x) while(x) continue;

FCliResult AnchorpointCliUtils::RunApCommand(const FString& InCommand, bool bRequestJsonOutput)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(RunApCli);

	FCliResult Result;

	FString IniConfigFile = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("ap-command.ini"));
	FString IniConfigContent = AnchorpointCliUtils::ConvertCommandToIni(InCommand);
	FFileHelper::SaveStringToFile(IniConfigContent, *(IniConfigFile));

	TSharedPtr<FMonitoredProcess> Process = nullptr;

	FString CommandLineExecutable = FAnchorpointCliModule::Get().GetCliPath();

	TArray<FString> Args;

	if (bRequestJsonOutput)
	{
		Args.Add(TEXT("--json"));
	}

	Args.Add(FString::Printf(TEXT("--config=\"%s\""), *IniConfigFile));

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

FCliResult AnchorpointCliUtils::RunGitCommand(const FString& InCommand)
{
	const FString GitViaAp = FString::Printf(TEXT("git --command \"%s\""), *InCommand);
	return RunApCommand(GitViaAp, false);
}
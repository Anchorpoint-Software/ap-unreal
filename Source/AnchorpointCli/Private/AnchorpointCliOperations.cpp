// Some copyright should be here...

#include "AnchorpointCliOperations.h"

#include "JsonObjectConverter.h"
#include "Misc/MonitoredProcess.h"

TSharedPtr<FMonitoredProcess> RunApCli(const FString& InCommand)
{
#if PLATFORM_MAC
	const FString CommandLineExecutable = "/Users/alexandruoprea/Desktop/Clients/Anchorpoint/Anchorpoint.app/Contents/Frameworks/ap";
#endif

	TArray<FString> Args;
	Args.Add(FString::Printf(TEXT("--cwd=\"%s\""), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));
	Args.Add(TEXT("--json"));

	Args.Add(InCommand);

	FString CommandLineArgs = FString::Join(Args, TEXT(" "));
	TSharedRef<FMonitoredProcess> Process = MakeShared<FMonitoredProcess>(CommandLineExecutable, CommandLineArgs, true);

	const bool bLaunchSuccess = Process->Launch();
	if(!bLaunchSuccess)
	{
		return nullptr;
	}

	while (Process->Update()) {}

	return Process;
}

TSharedPtr<FJsonObject> JsonStringToJsonObject(const FString& InJsonString)
{
	TSharedPtr<FJsonObject> JsonObject = nullptr;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(InJsonString);
	FJsonSerializer::Deserialize( Reader, JsonObject );

	return JsonObject;
}

TValueOrError<FString, FString> AnchorpointCliOperations::Connect()
{
	TSharedPtr<FMonitoredProcess> Process = RunApCli(TEXT("status"));
	if (!Process || Process->GetReturnCode() != 0)
	{
		return MakeError(TEXT("Failed to run CLI"));
	}

	const FString Output = Process->GetFullOutputWithoutDelegate();
	TSharedPtr<FJsonObject> Object = JsonStringToJsonObject(Output);

	if(!Object.IsValid())
	{
		return MakeError(FString::Printf(TEXT("Failed to parse output: %s"), *Output));
	}

	FString Error;
	if(Object->TryGetStringField(TEXT("error"), Error))
	{
		return MakeError(FString::Printf(TEXT("CLI error: %s"), *Output));
	}

	return MakeValue(TEXT("Success"));
}

TValueOrError<FString, FString> AnchorpointCliOperations::GetStatus()
{
	TSharedPtr<FMonitoredProcess> Process = RunApCli(TEXT("status"));
	if (!Process || Process->GetReturnCode() != 0)
	{
		return MakeError(TEXT("Failed to run CLI"));
	}

	const FString Output = Process->GetFullOutputWithoutDelegate();
	return MakeValue(Output);
}
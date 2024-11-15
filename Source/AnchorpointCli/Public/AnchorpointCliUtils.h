#pragma once

class FMonitoredProcess;

struct FCliResult
{
	FString Output;
	TOptional<FString> Error;

	bool DidSucceed() const;
	TSharedPtr<FJsonObject> OutputAsJsonObject() const;
	TArray<TSharedPtr<FJsonValue>> OutputAsJsonArray() const;
};

struct FCliParameters
{
	FCliParameters(const FString& InCommand);

	FString Command;
	bool bRequestJsonOutput;

	using FOnProcessUpdate = TFunction<bool(const TSharedPtr<FMonitoredProcess>& InProcess)>;
	FOnProcessUpdate OnProcessUpdate;
};

namespace AnchorpointCliUtils
{
	FString ConvertCommandToIni(const FString& InCommand, bool bPrintConfig = false, bool bJsonOutput = true);

	FCliResult RunApCommand(const FString& InCommand);
	FCliResult RunApCommand(const FCliParameters& InParameters);
	FCliResult RunGitCommand(const FString& InCommand);
}
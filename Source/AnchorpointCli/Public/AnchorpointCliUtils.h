#pragma once

struct FCliResult
{
	FString Output;
	TOptional<FString> Error;

	bool DidSucceed() const;
	TSharedPtr<FJsonObject> OutputAsJsonObject() const;
	TArray<TSharedPtr<FJsonValue>> OutputAsJsonArray() const;
};

namespace AnchorpointCliUtils
{
	FString ConvertCommandToIni(const FString& InCommand, bool bPrintConfig = false, bool bJsonOutput = true);

	FCliResult RunApCommand(const FString& InCommand, bool bRequestJsonOutput = true);
	FCliResult RunGitCommand(const FString& InCommand);
}
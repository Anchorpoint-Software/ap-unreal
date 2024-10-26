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
	FString ToRelativePath(const FString& InFullPath);

	FString ConvertCommandToIni(const FString& InCommand, bool bPrintConfig = false);

	FCliResult RunApCommand(const FString& InCommand, bool bRequestJsonOutput = true);
	FCliResult RunGitCommand(const FString& InCommand);
}
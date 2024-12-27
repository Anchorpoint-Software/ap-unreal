// Copyright (C) 2024-2025 Anchorpoint GmbH. All rights reserved.

#pragma once

#include "AnchorpointCliProcess.h"

class FAnchorpointCliProcess;

namespace AnchorpointCliCommands
{
	/**
	 * Helper function to convert a command string to an .ini file
	 * Note: This should yield the same results as `ap  --print-config`
	 */
	FString ConvertCommandToIni(const FString& InCommand, bool bPrintConfig = false, bool bJsonOutput = true);
	/**
	 * Runs an AP CLI command based on the input parameters
	 */
	FCliResult RunApCommand(const FCliParameters& InParameters);
	/**
	 * Helper function to facilitate running Git commands easier.
	 * Note: This is basically a wrapper for `ap git --command "<InCommand>"`
	 */
	FCliResult RunGitCommand(const FString& InCommand, bool bUseBinaryData = false);
}
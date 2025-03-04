﻿// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

/**
 * Enum containing the possible states a file from the Anchorpoint status can be in
 */
enum class EAnchorpointFileOperation
{
	Invalid,
	Added,
	Modified,
	Deleted,
	Renamed,
	Conflicted,
};

EAnchorpointFileOperation LexFromString(const FString& InString);

/**
 * Wrapper struct for all the data returned by the Anchorpoint status command
 */
struct ANCHORPOINTCLI_API FAnchorpointStatus
{
	/**
	 * Constructs a status struct from a json object
	 */
	static FAnchorpointStatus FromJson(const TSharedRef<FJsonObject>& InJsonObject);

	/**
	 * Git branch the project is currently on
	 */
	FString CurrentBranch;
	/**
	 * Map of all files that are staged for commit and their status
	 */
	TMap<FString, EAnchorpointFileOperation> Staged;
	/**
	 * Map of all files that are not staged for commit and their status
	 */
	TMap<FString, EAnchorpointFileOperation> NotStaged;
	/**
	 * Map of all files that are locked and the user that locked them
	 */
	TMap<FString, FString> Locked;
	/**
	 * List of all that are outdated and need to be updated
	 */
	TArray<FString> Outdated;

	/**
	 * Helper function to get all the files mentioned by any of the states above
	 */
	TArray<FString> GetAllAffectedFiles() const;
};

struct ANCHORPOINTCLI_API FAnchorpointHistoryEntry
{
	/**
	 * Constructs an array of history entries from a json array 
	 */
	static TArray<FAnchorpointHistoryEntry> ArrayFromJsonArray(const TArray<TSharedPtr<FJsonValue>>& HistoryEntries);

	/**
	 * SHA of the git commit 
	 */
	FString Commit;
	/**
	 * Author of the git commit
	 */
	FString Author;
	/**
	 * Message of the git commit
	 */
	FString Message;
	/**
	 * Date of the git commit
	 */
	FDateTime Date;
};

using FAnchorpointHistory = TArray<FAnchorpointHistoryEntry>;
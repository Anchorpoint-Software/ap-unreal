// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

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

/**
 * Copy of FGitConflictStatusParser for Anchorpoint
 * 
 * Extract the status of a unmerged (conflict) file
 *
 * Example output of git ls-files --unmerged Content/Blueprints/BP_Test.uasset
100644 d9b33098273547b57c0af314136f35b494e16dcb 1	Content/Blueprints/BP_Test.uasset
100644 a14347dc3b589b78fb19ba62a7e3982f343718bc 2	Content/Blueprints/BP_Test.uasset
100644 f3137a7167c840847cd7bd2bf07eefbfb2d9bcd2 3	Content/Blueprints/BP_Test.uasset
 *
 * 1: The "common ancestor" of the file (the version of the file that both the current and other branch originated from).
 * 2: The version from the current branch (the master branch in this case).
 * 3: The version from the other branch (the test branch)
 */
class ANCHORPOINTCLI_API FAnchorpointConflictStatus
{
public:
	/** Parse the unmerge status: extract the base SHA1 identifier of the file */
	FAnchorpointConflictStatus(const TArray<FString>& InResults)
	{
		const FString& CommonAncestor = InResults[0]; // 1: The common ancestor of merged branches
		CommonAncestorFileId = CommonAncestor.Mid(7, 40);
		CommonAncestorFilename = CommonAncestor.Right(50);

		if (ensure(InResults.IsValidIndex(2)))
		{
			const FString& RemoteBranch = InResults[2]; // 1: The common ancestor of merged branches
			RemoteFileId = RemoteBranch.Mid(7, 40);
			RemoteFilename = RemoteBranch.Right(50);
		}
	}

	FString CommonAncestorFileId; ///< SHA1 Id of the file (warning: not the commit Id)
	FString RemoteFileId; ///< SHA1 Id of the file (warning: not the commit Id)

	FString CommonAncestorFilename;
	FString RemoteFilename;
};
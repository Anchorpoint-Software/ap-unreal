// Copyright (C) 2024-2025 Anchorpoint GmbH. All rights reserved.

#pragma once

#include <ISourceControlRevision.h>

#include "AnchorpointCliStatus.h"

/**
 * Anchorpoint implementation of the SourceControl module Revision interface to represent a revision of a file in the history.
 */
class FAnchorpointSourceControlRevision : public ISourceControlRevision
{
public:
	explicit FAnchorpointSourceControlRevision(const FAnchorpointHistoryEntry& InHistoryEntry, const FString& InFilename, int32 InRevisionNumber);

	// ~ Begin ISourceControlRevision interface
	virtual bool Get(FString& InOutFilename, EConcurrency::Type InConcurrency = EConcurrency::Synchronous) const override;
	virtual bool GetAnnotated(TArray<FAnnotationLine>& OutLines) const override;
	virtual bool GetAnnotated(FString& InOutFilename) const override;
	virtual const FString& GetFilename() const override;
	virtual int32 GetRevisionNumber() const override;
	virtual const FString& GetRevision() const override;
	virtual const FString& GetDescription() const override;
	virtual const FString& GetUserName() const override;
	virtual const FString& GetClientSpec() const override;
	virtual const FString& GetAction() const override;
	virtual TSharedPtr<ISourceControlRevision> GetBranchSource() const override;
	virtual const FDateTime& GetDate() const override;
	virtual int32 GetCheckInIdentifier() const override;
	virtual int32 GetFileSize() const override;
	// ~ End ISourceControlRevision interface

	/**
	 * The *local* filename this revision refers to
	 */
	FString Filename;
	/**
	 * The size of the file at this revision
	 */
	int32 FileSize;
	/**
	 * The full hexadecimal SHA1 id of the commit this revision refers to
	 */
	FString CommitId;
	/**
	 * The numeric value of the short SHA1 (8 first hex char out of 40)
	 */
	int32 CommitIdNumber;
	/**
	 * The short hexadecimal SHA1 id (8 first hex char out of 40) of the commit: the string to display
	 */
	FString ShortCommitId;
	/**
	 * The index of the revision in the history (SBlueprintRevisionMenu assumes order for the "Depot" label)
	 */
	int32 RevisionNumber;
	/**
	 * The description of this revision
	 */
	FString Messsage;
	/**
	 * The user that made the change
	 */
	FString Author;
	/**
	 * The action (add, edit, branch etc.) performed at this revision
	 */
	FString Action;
	/**
	 * The date this revision was made
	 */
	FDateTime Date;
	/**
	 * Source of move ("branch" in Perforce term) if any
	 */
	TSharedPtr<FAnchorpointSourceControlRevision> BranchSource;
};

// All source control plugin use this typedef to simplify the workflow with these type of class. I feel the T prefix is wrong, but it's the convention.
using TAnchorpointSourceControlHistory = TArray<TSharedRef<FAnchorpointSourceControlRevision>>;
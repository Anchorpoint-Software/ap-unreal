// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

#include "AnchorpointSourceControlRevision.h"
#include "AnchorpointSourceControlState.h"
#include "AnchorpointSourceControlWorker.h"

struct FAnchorpointConnectMessage;

class FAnchorpointConnectWorker final : public IAnchorpointSourceControlWorker
{
public:
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	//~ End IAnchorpointSourceControlWorker Interface
};

class FAnchorpointCheckOutWorker final : public IAnchorpointSourceControlWorker
{
public:
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	virtual void TickWhileInProgress() override;
	//~ End IAnchorpointSourceControlWorker Interface

private:
	void OnCliConnectMessage(const FAnchorpointConnectMessage& Message);

	TArray<FAnchorpointSourceControlState> States;
	int NumFilesLockedSinceStart = 0;
};

class FAnchorpointRevertWorker final : public IAnchorpointSourceControlWorker
{
public:
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	//~ End IAnchorpointSourceControlWorker Interface
};

class FAnchorpointUpdateStatusWorker final : public IAnchorpointSourceControlWorker
{
public:
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	//~ End IAnchorpointSourceControlWorker Interface

	TArray<FAnchorpointSourceControlState> States;

	TMap<FString, TAnchorpointSourceControlHistory> Histories;
	TMap<FString, FAnchorpointConflictStatus> Conflicts;
};

class FAnchorpointAddWorker final : public IAnchorpointSourceControlWorker
{
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	//~ End IAnchorpointSourceControlWorker Interface
};

class FAnchorpointCopyWorker final : public IAnchorpointSourceControlWorker
{
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	//~ End IAnchorpointSourceControlWorker Interface
};

class FAnchorpointDeleteWorker final : public IAnchorpointSourceControlWorker
{
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	//~ End IAnchorpointSourceControlWorker Interface
};

class FAnchorpointCheckInWorker final : public IAnchorpointSourceControlWorker
{
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	//~ End IAnchorpointSourceControlWorker Interface
};

class FAnchorpointDownloadFileWorker final : public IAnchorpointSourceControlWorker
{
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	//~ End IAnchorpointSourceControlWorker Interface
};

class FAnchorpointResolveWorker final : public IAnchorpointSourceControlWorker
{
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	//~ End IAnchorpointSourceControlWorker Interface

	TArray< FString > UpdatedFiles;
};
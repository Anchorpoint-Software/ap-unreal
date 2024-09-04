// Some copyright should be here...

#pragma once

#include "AnchorpointControlState.h"
#include "AnchorpointSourceControlWorker.h"

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
	//~ End IAnchorpointSourceControlWorker Interface
	
	TArray<FAnchorpointControlState> States;
};

class FAnchorpointRevertWorker final : public IAnchorpointSourceControlWorker
{
public:
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	//~ End IAnchorpointSourceControlWorker Interface
	
	TArray<FAnchorpointControlState> States;
};

class FAnchorpointUpdateStatusWorker final : public IAnchorpointSourceControlWorker
{
public:
	//~ Begin IAnchorpointSourceControlWorker Interface
	virtual FName GetName() const override;
	virtual bool Execute(FAnchorpointSourceControlCommand& InCommand) override;
	virtual bool UpdateStates() const override;
	//~ End IAnchorpointSourceControlWorker Interface

	TArray<FAnchorpointControlState> States;
};
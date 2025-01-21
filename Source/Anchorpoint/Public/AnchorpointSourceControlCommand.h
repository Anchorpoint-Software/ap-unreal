// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

#include <ISourceControlProvider.h>
#include <Misc/IQueuedWork.h>

class IAnchorpointSourceControlWorker;

/**
 * Used to execute Anchorpoint commands multi-threaded.
 */
class FAnchorpointSourceControlCommand : public IQueuedWork
{
public:
	FAnchorpointSourceControlCommand(const TSharedRef<ISourceControlOperation>& InOperation, const TSharedRef<IAnchorpointSourceControlWorker>& InWorker, const FSourceControlOperationComplete& InOperationCompleteDelegate = FSourceControlOperationComplete());

	/**
	 * This is where the real thread work is done. All work that is done for
	 * this queued object should be done from within the call to this function.
	 */
	bool DoWork();

	/**
	 * Tells the queued work that it is being abandoned so that it can do
	 * per object clean up as needed. This will only be called if it is being
	 * abandoned before completion. NOTE: This requires the object to delete
	 * itself using whatever heap it was allocated in.
	 */
	virtual void Abandon() override;

	/**
	 * This method is also used to tell the object to cleanup but not before
	 * the object has finished it's work.
	 */
	virtual void DoThreadedWork() override;

	/**
	 * Save any results and call any registered callbacks.
	 */
	ECommandResult::Type ReturnResults();

	/**
	 * Operation we want to perform - contains outward-facing parameters & results
	 */
	TSharedRef<ISourceControlOperation> Operation;

	/**
	 * The object that will actually do the work
	 */
	TSharedRef<IAnchorpointSourceControlWorker> Worker;

	/**
	 * Delegate to notify when this operation completes
	 */
	FSourceControlOperationComplete OperationCompleteDelegate;

	/**
	 * If true, this command has been processed by the source control thread
	 */
	volatile int32 bExecuteProcessed;

	/**
	 * If true, the source control command succeeded
	 */
	bool bCommandSuccessful;

	/**
	 * If true, this command will be automatically cleaned up in Tick()
	 */
	bool bAutoDelete;

	/**
	 * Whether we are running multi-treaded or not
	 */
	EConcurrency::Type Concurrency;

	/**
	 * Files to perform this operation on
	 */
	TArray<FString> Files;

	/**
	 * Info and/or warning message storage
	 */
	TArray<FString> InfoMessages;

	/**
	 * Potential error message storage
	 */
	TArray<FString> ErrorMessages;
};
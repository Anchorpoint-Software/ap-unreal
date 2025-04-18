﻿// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

/**
 * Base class for all the SourceControl workers performing operations.
 */
class IAnchorpointSourceControlWorker
{
public:
	virtual ~IAnchorpointSourceControlWorker() = default;
	/**
	 * Name describing the work that this worker does. Used for factory method hookup.
	 */
	virtual FName GetName() const = 0;
	/**
	 * Function that actually does the work. Can be executed on another thread.
	 */
	virtual bool Execute(class FAnchorpointSourceControlCommand& InCommand) = 0;
	/**
	 * Updates the state of any items after completion (if necessary). This is always executed on the main thread.
	 * @returns true if states were updated
	 */
	virtual bool UpdateStates() const = 0;
	/**
	 * Allows to perform some work while the command is in progress. This is always executed on the main thread.
	 */
	virtual void TickWhileInProgress() {};
};
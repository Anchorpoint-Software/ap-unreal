// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

class FAnchorpointCliProcess;

/**
 * Structure representing the result of a CLI command with helper functions to parse the output
 */
struct FCliResult
{
	/**
	 * String data from the stdout pipe
	 */
	FString StdOutOutput;
	/**
	 * String data from the stderr pipe
	 */
	FString StdErrOutput;
	/**
	 * Binary data from the stdout pipe
	 */
	TArray<uint8> StdOutBinary;
	/**
	 * Binary data from the stderr pipe
	 */
	TArray<uint8> StdErrBinary;
	/**
	 * Exit code of the process. If unset, the process was still running when the result was retrieved
	 */
	TOptional<int> ReturnCode;
	/**
	 * Helper function to determine if the command was successfully run
	 * Note: This does not mean the command itself was successful, only that the process completed successfully
	 */
	bool DidSucceed() const;
	/**
	 * Helper function to retrieve the parsed output as a JSON object
	 */
	TSharedPtr<FJsonObject> OutputAsJsonObject() const;
	/**
	 * Helper function to retrieve the parsed output as an array of JSON objects
	 */
	TArray<TSharedPtr<FJsonValue>> OutputAsJsonArray() const;
	/**
	 * Gets the most relevant error message possible, in order:
	 * 1. If the CLI printed a message with "error" -> returns the value
	 * 2. If the CLI exited with a non-0 code -> returns error for exit code
	 *
	 * Otherwise, empty string
	 */
	FString GetBestError() const;
};

/**
 * Structure representing all the configurable parameters for running a CLI command
 */
struct FCliParameters
{
	FCliParameters(const FString& InCommand);

	/**
	 * The AP CLI command we should run with all the necessary parameters
	 */
	FString Command;
	/**
	 * If true, the command will be run using the `-config` flag using an .ini file
	 */
	bool bUseIniFile = true;
	/**
	 * If true, the command will be run using the `-json` flag
	 */
	bool bRequestJsonOutput = true;
	/**
	 * If true, the process output will be read as binary data
	 */
	bool bUseBinaryOutput = false;
	/**
	 * Callback executed to determine if we should terminate the process early.
	 * If the caller wants to terminate early, they should return true
	 */
	using FOnProcessUpdate = TFunction<bool(const TSharedRef<FAnchorpointCliProcess>& InProcess, const FString& ProcessOutput)>;
	FOnProcessUpdate OnProcessUpdate;
};

/**
 * Struct representing the output data from a CLI process
 */
struct FAnchorpointCliProcessOutputData
{
	/**
	 * Decides if we should read the data from the pipe as binary or string
	 */
	bool bUseBinaryData = false;

	/**
	 * All the string data we read from the pipe
	 */
	FString StringData;
	/**
	 * All the binary data we read from the pipe
	 */
	TArray<uint8> BinaryData;
	/**
	 * Performs the actual reading of the data from the pipe, depending on the type of data
	 */
	void ReadData(void* ReadPipe);
};

class FAnchorpointCliProcess : public FRunnable
{
public:
	virtual ~FAnchorpointCliProcess() override;

	/**
	 * Starts the executable process with certain launch parameters
	 * Note: You can start a process directly via exe & args or provide extra parameters via the FCliParameters struct
	 */
	bool Launch(const FCliParameters& InParameters);
	bool Launch(const FString& Executable, const FString& Parameters);
	/**
	 * Checks whether the process is still running
	 */
	bool IsRunning() const;
	/**
	 * Returns the return code from the exited process
	 */
	FCliResult GetResult();
	/**
	 * Cancels the process by killing the entire process tree.
	 */
	void Cancel();

	/**
	 * Sends the string message when process is ready
	 */
	void SendWhenReady(const FString& Message);

	void GetStdOutData(FString& OutStringData, TArray<uint8>& OutBinaryData);
	void GetStdErrData(FString& OutStringData, TArray<uint8>& OutBinaryData);
	void ClearStdOutData();
	void ClearStdErrData();

	using FOnProcessEvent = TMulticastDelegate<void()>;
	FOnProcessEvent OnProcessStarted;
	FOnProcessEvent OnProcessUpdated;
	FOnProcessEvent OnProcessEnded;

private:
	// ~Begin FRunnable Interface
	virtual uint32 Run() override;
	virtual void Stop() override;
	// ~End FRunnable Interface

	void TickInternal();

	void SendQueuedMessages();

	TQueue<FString> MessagesToSend;

	FProcHandle ProcessHandle;
	TSAN_ATOMIC(bool) bIsRunning = false;
	bool bIsCanceling = false;

	/**
	 * If unset, the process might still be running
	 */
	TOptional<int> ReturnCode;

	void* PipeOutputRead = nullptr;
	void* PipeOutputWrite = nullptr;
	void* PipeInputRead = nullptr;
	void* PipeInputWrite = nullptr;
	void* PipeOutputReadErr = nullptr;
	void* PipeOutputWriteErr = nullptr;

	FCriticalSection OutputLock;
	FAnchorpointCliProcessOutputData StdOutData;
	FAnchorpointCliProcessOutputData StdErrData;

	FRunnableThread* Thread = nullptr;
};
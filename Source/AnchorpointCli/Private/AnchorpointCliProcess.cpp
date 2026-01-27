// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointCliProcess.h"

#include "AnchorpointCli.h"
#include "AnchorpointCliCommands.h"
#include "AnchorpointCliLog.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#endif

FString ParseOutputDirect(const TArray<uint8>& RawOutput)
{
	FString TestOutput;
	for (int i = 0; i < RawOutput.Num(); i++)
	{
		const TCHAR CurrentChar = RawOutput[i];
		TestOutput.AppendChar(CurrentChar);
	}
	TestOutput.TrimEndInline();

	return TestOutput;
}

void FAnchorpointCliProcessOutputData::ReadData(void* InReadPipe)
{
	if (bUseBinaryData)
	{
		TArray<uint8> NewData;
		FPlatformProcess::ReadPipeToArray(InReadPipe, NewData);

		BinaryData.Append(MoveTemp(NewData));
	}
	else
	{
		TArray<uint8> RawOutput;
		FPlatformProcess::ReadPipeToArray(InReadPipe, RawOutput);
		const FString NewOutput = ParseOutputDirect(RawOutput);

		if (NewOutput.IsEmpty())
		{
			return;
		}

		if (StringData.IsEmpty())
		{
			StringData = NewOutput;
		}
		else
		{
			StringData.Appendf(TEXT("%s%s"), LINE_TERMINATOR, *NewOutput);
		}
	}
}

FAnchorpointCliProcess::~FAnchorpointCliProcess()
{
	if (ProcessHandle.IsValid())
	{
		Cancel();
	}

	if (Thread && IsRunning())
	{
		// Similar to FInteractiveProcess::~FInteractiveProcess,
		// we only wait for completion if the underlying process is running
		Thread->WaitForCompletion();
		delete Thread;
	}
}

bool FAnchorpointCliProcess::Launch(const FCliParameters& InParameters)
{
	const FAnchorpointCliModule* Module = FAnchorpointCliModule::GetPtr();
	if (!Module)
	{
		UE_LOG(LogAnchorpointCli, Error, TEXT("AnchorpointCli module is unavailable. Cannot launch CLI process."));
		return false;
	}

	const FString CommandLineExecutable = Module->GetCliPath();
	FString CommandLineArgs;

	if (InParameters.bUseIniFile)
	{
		FString IniConfigFile = FPaths::EngineIntermediateDir() / TEXT("Anchorpoint") / TEXT("ap-command.ini");
		FString IniConfigContent = AnchorpointCliCommands::ConvertCommandToIni(InParameters.Command, false, InParameters.bRequestJsonOutput);
		FFileHelper::SaveStringToFile(IniConfigContent, *IniConfigFile, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);

		CommandLineArgs = FString::Printf(TEXT("--config=\"%s\""), *IniConfigFile);
		UE_LOG(LogAnchorpointCli, Verbose, TEXT("Ini content: %s"), *IniConfigContent);
	}
	else
	{
		TArray<FString> Args;
		Args.Add(FString::Printf(TEXT("--cwd=\"%s\""), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));

		if (InParameters.bRequestJsonOutput)
		{
			Args.Add(TEXT("--json"));
		}

		Args.Add(TEXT("--apiVersion 1"));
		Args.Add(InParameters.Command);

		CommandLineArgs = FString::Join(Args, TEXT(" "));
	}

	StdOutData.bUseBinaryData = InParameters.bUseBinaryOutput;
	StdErrData.bUseBinaryData = InParameters.bUseBinaryOutput;

	return Launch(CommandLineExecutable, CommandLineArgs);
}

bool FAnchorpointCliProcess::Launch(const FString& Executable, const FString& Parameters)
{
	checkf(FPlatformProcess::CreatePipe(PipeOutputRead, PipeOutputWrite, false), TEXT("Failed to create pipe for stdout"));
	checkf(FPlatformProcess::CreatePipe(PipeInputRead, PipeInputWrite, true), TEXT("Failed to create pipe for stdout"));
	checkf(FPlatformProcess::CreatePipe(PipeOutputReadErr, PipeOutputWriteErr, false), TEXT("Failed to create pipe for stderr"));

	constexpr bool bLaunchDetached = false;
	constexpr bool bLaunchHidden = true;
	constexpr bool bLaunchReallyHidden = bLaunchHidden;

	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Running %s %s"), *Executable, *Parameters);
	ProcessHandle = FPlatformProcess::CreateProc(*Executable, *Parameters, bLaunchDetached, bLaunchHidden, bLaunchReallyHidden, nullptr, 0, nullptr, PipeOutputWrite, PipeInputRead, PipeOutputWriteErr);
	if (!ProcessHandle.IsValid())
	{
		return false;
	}

	static std::atomic<uint32> AnchorpointProcessIndex{0};
	const FString ProcessName = FString::Printf(TEXT("AnchorpointProcessIndex %d"), AnchorpointProcessIndex.fetch_add(1));

	Thread = FRunnableThread::Create(this, *ProcessName, 128 * 1024, TPri_AboveNormal);

	return true;
}

bool FAnchorpointCliProcess::IsRunning() const
{
	return !bIsRunFinished;
}

FCliResult FAnchorpointCliProcess::GetResult()
{
	FCliResult Result;
	Result.ReturnCode = ReturnCode;

	GetStdOutData(Result.StdOutOutput, Result.StdOutBinary);
	GetStdErrData(Result.StdErrOutput, Result.StdErrBinary);

	return Result;
}

void FAnchorpointCliProcess::Cancel()
{
	bIsCanceling = true;
}

void FAnchorpointCliProcess::SendWhenReady(const FString& Message)
{
	MessagesToSend.Enqueue(Message);
}

void FAnchorpointCliProcess::GetStdOutData(FString& OutStringData, TArray<uint8>& OutBinaryData)
{
	FScopeLock ScopeLock(&OutputLock);
	OutStringData = StdOutData.StringData;
	OutBinaryData = StdOutData.BinaryData;
}

void FAnchorpointCliProcess::GetStdErrData(FString& OutStringData, TArray<uint8>& OutBinaryData)
{
	FScopeLock ScopeLock(&OutputLock);
	OutStringData = StdErrData.StringData;
	OutBinaryData = StdErrData.BinaryData;
}

void FAnchorpointCliProcess::ClearStdOutData()
{
	FScopeLock ScopeLock(&OutputLock);
	StdOutData.StringData.Empty();
	StdOutData.BinaryData.Empty();
}

void FAnchorpointCliProcess::ClearStdErrData()
{
	FScopeLock ScopeLock(&OutputLock);
	StdErrData.StringData.Empty();
	StdErrData.BinaryData.Empty();
}

uint32 FAnchorpointCliProcess::Run()
{
	OnProcessStarted.Broadcast();

	while (ProcessHandle.IsValid())
	{
		constexpr float SleepInterval = 0.01f;
		FPlatformProcess::Sleep(SleepInterval);
		TickInternal();

		OnProcessUpdated.Broadcast();
	}

	FPlatformProcess::ClosePipe(PipeOutputRead, PipeOutputWrite);
	PipeOutputRead = PipeOutputWrite = nullptr;

	FPlatformProcess::ClosePipe(PipeInputRead, PipeInputWrite);
	PipeInputRead = PipeInputWrite = nullptr;

	FPlatformProcess::ClosePipe(PipeOutputReadErr, PipeOutputWriteErr);
	PipeOutputReadErr = PipeOutputWriteErr = nullptr;

	OnProcessEnded.Broadcast();
	return 0;
}

void FAnchorpointCliProcess::Stop()
{
	Cancel();
}

void FAnchorpointCliProcess::Exit()
{
	bIsRunFinished = true;
}

void FAnchorpointCliProcess::TickInternal()
{
	{
		FScopeLock ScopeLock(&OutputLock);
		StdOutData.ReadData(PipeOutputRead);
		StdErrData.ReadData(PipeOutputReadErr);
	}

	SendQueuedMessages();

	if (bIsCanceling)
	{
		FPlatformProcess::TerminateProc(ProcessHandle, true);
		ProcessHandle.Reset();
	}
	else if (!FPlatformProcess::IsProcRunning(ProcessHandle))
	{
		int ProcessReturnCode = -1;
		if (!FPlatformProcess::GetProcReturnCode(ProcessHandle, &ProcessReturnCode))
		{
			ReturnCode = -1;
		}

		ReturnCode = ProcessReturnCode;
		ProcessHandle.Reset();
	}
}

void FAnchorpointCliProcess::SendQueuedMessages()
{
	if (!MessagesToSend.IsEmpty())
	{
		FString MessageToSend, MessageSent;
		MessagesToSend.Dequeue(MessageToSend);

		FPlatformProcess::WritePipe(PipeInputWrite, MessageToSend, &MessageSent);

		UE_LOG(LogAnchorpointCli, Log, TEXT("MessageToSend: \n%s\nMessageSent: \n%s\n"), *MessageToSend, *MessageSent);

		UE_CLOG(MessageSent.Len() == 0, LogAnchorpointCli, Error, TEXT("Writing message through pipe failed"));
		UE_CLOG(MessageToSend.Len() > MessageSent.Len(), LogAnchorpointCli, Error, TEXT("Writing some part of the message through pipe failed"));
	}
}
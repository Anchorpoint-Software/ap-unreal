// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointCliProcess.h"

#include "AnchorpointCli.h"
#include "AnchorpointCliCommands.h"
#include "AnchorpointCliLog.h"

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
		FString NewOutput = FPlatformProcess::ReadPipe(InReadPipe);
		NewOutput.TrimEndInline();

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
	if (bIsRunning)
	{
		Cancel();
	}

	if (Thread != nullptr)
	{
		Thread->WaitForCompletion();
		delete Thread;
	}
}

bool FAnchorpointCliProcess::Launch(const FCliParameters& InParameters)
{
	FString CommandLineExecutable = FAnchorpointCliModule::Get().GetCliPath();
	FString CommandLineArgs;

	if (InParameters.bUseIniFile)
	{
		FString IniConfigFile = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("ap-command.ini"));
		FString IniConfigContent = AnchorpointCliCommands::ConvertCommandToIni(InParameters.Command, false, InParameters.bRequestJsonOutput);
		FFileHelper::SaveStringToFile(IniConfigContent, *(IniConfigFile));

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
	checkf(!bIsRunning, TEXT("Process is already running"));
	checkf(FPlatformProcess::CreatePipe(PipeRead, PipeWrite), TEXT("Failed to create pipe for stdout"));
	checkf(FPlatformProcess::CreatePipe(PipeReadErr, PipeWriteErr), TEXT("Failed to create pipe for stderr"));

	constexpr bool bLaunchDetached = false;
	constexpr bool bLaunchHidden = true;
	constexpr bool bLaunchReallyHidden = bLaunchHidden;

	UE_LOG(LogAnchorpointCli, Verbose, TEXT("Running %s %s"), *Executable, *Parameters);
	ProcessHandle = FPlatformProcess::CreateProc(*Executable, *Parameters, bLaunchDetached, bLaunchHidden, bLaunchReallyHidden, nullptr, 0, nullptr, PipeWrite, PipeRead, PipeWriteErr);
	if (!ProcessHandle.IsValid())
	{
		return false;
	}

	static std::atomic<uint32> AnchorpointProcessIndex{0};
	const FString ProcessName = FString::Printf(TEXT("AnchorpointProcessIndex %d"), AnchorpointProcessIndex.fetch_add(1));

	bIsRunning = true;
	Thread = FRunnableThread::Create(this, *ProcessName, 128 * 1024, TPri_AboveNormal);

	return true;
}

bool FAnchorpointCliProcess::IsRunning() const
{
	return bIsRunning;
}

FCliResult FAnchorpointCliProcess::GetResult()
{
	FCliResult Result;
	Result.ReturnCode = ReturnCode;

	if (ReturnCode != 0)
	{
		Result.Error = FString::Printf(TEXT("Process failed with code %d"), ReturnCode);
	}
	else
	{
		GetStdOutData(Result.StdOutOutput, Result.StdOutBinary);
		GetStdErrData(Result.StdErrOutput, Result.StdErrBinary);
	}

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

	while (bIsRunning)
	{
		constexpr float SleepInterval = 0.01f;
		FPlatformProcess::Sleep(SleepInterval);
		TickInternal();

		OnProcessUpdated.Broadcast();
	}

	FPlatformProcess::ClosePipe(PipeRead, PipeWrite);
	PipeRead = PipeWrite = nullptr;

	FPlatformProcess::ClosePipe(PipeReadErr, PipeWriteErr);
	PipeReadErr = PipeWriteErr = nullptr;


	OnProcessEnded.Broadcast();
	return 0;
}

void FAnchorpointCliProcess::Stop()
{
	Cancel();
}


void FAnchorpointCliProcess::TickInternal()
{
	{
		FScopeLock ScopeLock(&OutputLock);
		StdOutData.ReadData(PipeRead);
		StdErrData.ReadData(PipeReadErr);
	}

	SendQueuedMessages();

	if (bIsCanceling)
	{
		FPlatformProcess::TerminateProc(ProcessHandle, true);
		bIsRunning = false;
	}
	else if (!FPlatformProcess::IsProcRunning(ProcessHandle))
	{
		if (!FPlatformProcess::GetProcReturnCode(ProcessHandle, &ReturnCode))
		{
			ReturnCode = -1;
		}

		bIsRunning = false;
	}
}

void FAnchorpointCliProcess::SendQueuedMessages()
{
	if (!MessagesToSend.IsEmpty())
	{
		FString MessageToSend, MessageSent;
		MessagesToSend.Dequeue(MessageToSend);

		FPlatformProcess::WritePipe(PipeWrite, MessageToSend, &MessageSent);

		UE_LOG(LogAnchorpointCli, Log, TEXT("MessageToSend: \n%s\n MessageSent: \n%s\n"), *MessageToSend, *MessageSent);

		UE_CLOG(MessageSent.Len() == 0, LogAnchorpointCli, Error, TEXT("Writing message through pipe failed"));
		UE_CLOG(MessageToSend.Len() > MessageSent.Len(), LogAnchorpointCli, Error, TEXT("Writing some part of the message through pipe failed"));
	}
}
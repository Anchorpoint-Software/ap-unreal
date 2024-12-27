// Copyright (C) 2024-2025 Anchorpoint GmbH. All rights reserved.

#include "AnchorpointCliProcess.h"

#include "AnchorpointCli.h"
#include "AnchorpointCliCommands.h"
#include "AnchorpointCliLog.h"

/*
FCliResult AnchorpointCliProcess::LaunchProcessWithBinaryOutput(const FString& Executable, const FString& Parameters)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AnchorpointCliProcess::LaunchProcessWithBinaryOutput);

	FCliResult Result;

	int32 ReturnCode = -1;
	FString FullCommand;

	if (ProcessHandle.IsValid())
	{
		FPlatformProcess::Sleep(0.01f);

		while (FPlatformProcess::IsProcRunning(ProcessHandle))
		{
			TArray<uint8> BinaryData;
			FPlatformProcess::ReadPipeToArray(PipeRead, BinaryData);
			if (BinaryData.Num() > 0)
			{
				Result.Output.Append(MoveTemp(BinaryData));
			}
		}

		TArray<uint8> BinaryData;
		FPlatformProcess::ReadPipeToArray(PipeRead, BinaryData);
		if (BinaryData.Num() > 0)
		{
			checkf(false, TEXT("Should we ever have output after the process is done?"));
			Result.Output.Append(MoveTemp(BinaryData));
		}

		FPlatformProcess::GetProcReturnCode(ProcessHandle, &ReturnCode);
		FPlatformProcess::CloseProc(ProcessHandle);
	}
	else
	{
		Result.Error = TEXT("Failed to create process");
		return Result;
	}


	FPlatformProcess::ClosePipe(PipeRead, PipeWrite);
	FPlatformProcess::ClosePipe(PipeReadErr, PipeWriteErr);

	return Result;
}
*/

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

		if (NewOutput.Contains(TEXT("Waiting for Anchorpoint Daemon")))
		{
			UE_LOG(LogAnchorpointCli, Warning, TEXT("Discarding daemon message: %s"), *NewOutput);
			return;
		}

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
		Result.StdOutOutput = GetStdOutData().StringData;
		Result.StdErrOutput = GetStdErrData().StringData;
		Result.StdOutBinary = GetStdOutData().BinaryData;
		Result.StdErrBinary = GetStdErrData().BinaryData;
	}

	return Result;
}

void FAnchorpointCliProcess::Cancel()
{
	bIsCanceling = true;
}

FAnchorpointCliProcessOutputData FAnchorpointCliProcess::GetStdOutData()
{
	FScopeLock ScopeLock(&OutputLock);
	return StdOutData;
}

FAnchorpointCliProcessOutputData FAnchorpointCliProcess::GetStdErrData()
{
	FScopeLock ScopeLock(&OutputLock);
	return StdErrData;
}

uint32 FAnchorpointCliProcess::Run()
{
	while (bIsRunning)
	{
		constexpr float SleepInterval = 0.01f;
		FPlatformProcess::Sleep(SleepInterval);
		TickInternal();
	}

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

	if (bIsCanceling)
	{
		FPlatformProcess::TerminateProc(ProcessHandle, true);
		// TODO: Do we need a cancel delegate now?
		// CanceledDelegate.ExecuteIfBound();
		bIsRunning = false;
	}
	else if (!FPlatformProcess::IsProcRunning(ProcessHandle))
	{
		// close output pipes
		FPlatformProcess::ClosePipe(PipeRead, PipeWrite);
		PipeRead = PipeWrite = nullptr;

		FPlatformProcess::ClosePipe(PipeReadErr, PipeWriteErr);
		PipeReadErr = PipeWriteErr = nullptr;

		// get completion status
		if (!FPlatformProcess::GetProcReturnCode(ProcessHandle, &ReturnCode))
		{
			ReturnCode = -1;
		}

		// TODO: Do we need a completion delegate now?
		// CompletedDelegate.ExecuteIfBound(ReturnCode);
		bIsRunning = false;
	}
}
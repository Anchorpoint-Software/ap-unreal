// Some copyright should be here...

#include "AnchorpointControlCommand.h"

#include "AnchorpointSourceControlWorker.h"

FAnchorpointSourceControlCommand::FAnchorpointSourceControlCommand(const TSharedRef<ISourceControlOperation>& InOperation,
                                                                   const TSharedRef<IAnchorpointSourceControlWorker>& InWorker,
                                                                   const FSourceControlOperationComplete& InOperationCompleteDelegate)
	: Operation(InOperation)
	  , Worker(InWorker)
	  , OperationCompleteDelegate(InOperationCompleteDelegate)
	  , bExecuteProcessed(0)
	  , bCommandSuccessful(false)
	  , bAutoDelete(true)
	  , Concurrency(EConcurrency::Synchronous)
{
	// grab the providers settings here, so we don't access them once the worker thread is launched
	check(IsInGameThread());
	//FAnchorpointSourceControlModule& AnchorpointSourceControl = FModuleManager::LoadModuleChecked<FAnchorpointSourceControlModule>("AnchorpointSourceControl");
	//PathToAnchorpointBinary = AnchorpointSourceControl.AccessSettings().GetBinaryPath();
	//PathToRepositoryRoot = AnchorpointSourceControl.GetProvider().GetPathToRepositoryRoot();
}

bool FAnchorpointSourceControlCommand::DoWork()
{
	bCommandSuccessful = Worker->Execute(*this);
	FPlatformAtomics::InterlockedExchange(&bExecuteProcessed, 1);

	return bCommandSuccessful;
}

void FAnchorpointSourceControlCommand::Abandon()
{
	FPlatformAtomics::InterlockedExchange(&bExecuteProcessed, 1);
}

void FAnchorpointSourceControlCommand::DoThreadedWork()
{
	Concurrency = EConcurrency::Asynchronous;
	DoWork();
}

ECommandResult::Type FAnchorpointSourceControlCommand::ReturnResults()
{
	// Save any messages that have accumulated
	for (FString& String : InfoMessages)
	{
		Operation->AddInfoMessge(FText::FromString(String));
	}
	for (FString& String : ErrorMessages)
	{
		Operation->AddErrorMessge(FText::FromString(String));
	}

	// run the completion delegate if we have one bound
	ECommandResult::Type Result = bCommandSuccessful ? ECommandResult::Succeeded : ECommandResult::Failed;
	(void)OperationCompleteDelegate.ExecuteIfBound(Operation, Result);

	return Result;
}
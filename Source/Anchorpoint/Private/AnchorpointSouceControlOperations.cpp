// Some copyright should be here...

#include "AnchorpointSouceControlOperations.h"

#include "Anchorpoint.h"
#include "AnchorpointCommunication.h"
#include "AnchorpointCommunicationSubsystem.h"
#include "AnchorpointControlCommand.h"
#include "AnchorpointControlState.h"
#include "AnchorpointSourceControlProvider.h"
#include "SourceControlOperations.h"

bool UpdateCachedStates(const TArray<FAnchorpointControlState>& InStates)
{
	FAnchorpointSourceControlProvider& Provider = FAnchorpointModule::Get().GetProvider();

	for (const FAnchorpointControlState& InState : InStates)
	{
		TSharedRef<FAnchorpointControlState> State = Provider.GetStateInternal(InState.LocalFilename);
		*State = InState;
		State->TimeStamp = FDateTime::Now();
	}

	return !InStates.IsEmpty();
}


FName FAnchorpointConnectWorker::GetName() const
{
	return "Connect";
}

bool FAnchorpointConnectWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	UAnchorpointCommunicationSubsystem* Subsystem = GEditor->GetEditorSubsystem<UAnchorpointCommunicationSubsystem>();
	const bool bSuccess = Subsystem ? Subsystem->OpenDesktopApp() : false;
	return bSuccess;
}

bool FAnchorpointConnectWorker::UpdateStates() const
{
	return true;
}

FName FAnchorpointCheckOutWorker::GetName() const
{
	return "CheckOut";
}

bool FAnchorpointCheckOutWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	return true;
}

bool FAnchorpointCheckOutWorker::UpdateStates() const
{
	return true;
}

FName FAnchorpointUpdateStatusWorker::GetName() const
{
	return "UpdateStatus";
}

bool FAnchorpointUpdateStatusWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	check(InCommand.Operation->GetName() == GetName());

	TSharedRef<FUpdateStatus> Operation = StaticCastSharedRef<FUpdateStatus>(InCommand.Operation);

	const TArray<FString> Files = InCommand.Files;
	InCommand.bCommandSuccessful = AnchorpointCommunication::RunUpdateStatus(InCommand.Files, InCommand.ErrorMessages, States);

	return InCommand.bCommandSuccessful;
}

bool FAnchorpointUpdateStatusWorker::UpdateStates() const
{
	return UpdateCachedStates(States);
}
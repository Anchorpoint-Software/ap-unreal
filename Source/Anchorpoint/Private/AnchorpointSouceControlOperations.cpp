// Some copyright should be here...

#include "AnchorpointSouceControlOperations.h"

#include "AnchorpointCommunicationSubsystem.h"


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
	return true;
}

bool FAnchorpointUpdateStatusWorker::UpdateStates() const
{
	return true;
}
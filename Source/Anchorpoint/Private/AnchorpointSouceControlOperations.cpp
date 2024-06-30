// Some copyright should be here...

#include "AnchorpointSouceControlOperations.h"

FName FAnchorpointConnectWorker::GetName() const
{
	return "Connect";
}

bool FAnchorpointConnectWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	return true;
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
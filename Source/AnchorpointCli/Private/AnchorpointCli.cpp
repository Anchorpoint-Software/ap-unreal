// Some copyright should be here...

#include "AnchorpointCli.h"

FAnchorpointCliModule& FAnchorpointCliModule::Get()
{
	return FModuleManager::LoadModuleChecked<FAnchorpointCliModule>("AnchorpointCli");
}

FString FAnchorpointCliModule::GetCliPath() const
{
	if(GetCliPathDelegate.IsBound())
	{
		return GetCliPathDelegate.Execute();
	}

	return {};
}

IMPLEMENT_MODULE(FAnchorpointCliModule, AnchorpointCli)

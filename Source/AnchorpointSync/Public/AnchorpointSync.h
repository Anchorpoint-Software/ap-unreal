// Some copyright should be here...

#pragma once

#include <Modules/ModuleInterface.h>

class USubscription;

namespace apsync
{
	class Api;
}

class FAnchorpointSyncModule final : public IModuleInterface
{
	//~ Begin IModuleInterface Interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface

	void ApiExample() const;

	std::shared_ptr<apsync::Api> _Api = nullptr;
	USubscription* _SubscriptionHandler = nullptr;
};

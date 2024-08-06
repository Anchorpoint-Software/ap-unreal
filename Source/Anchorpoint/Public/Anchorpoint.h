// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Subscription.h"

#include "AnchorpointSourceControlProvider.h"

/**

	Anchorpoint in Unreal Engine does 3 things from a technical point of view.
	
	1) IPC from Anchorpoint to Unreal Engine
	2) Implement a Source Control Provider using IPC from Unreal Engine to Anchorpoint
	3) Show state in Unreal, e.g. Locked Files and lock files automatically on change

	Use Cases:

	1)	We see errors when Anchorpoint is pulling data from Git while Unreal Engine is running due to loaded packages. 
		UE prevents other applications to apply changes to loaded packages, in Git we see an "unable to unlink error".
		Other Plugins (e.g. Perforce) call UnlinkPackages to prevent that.
		If the user clicks PULL in Anchorpoint, Unreal Engine should be noticed and Packages should be unlinked as well.

	2)	The user should be able to PUSH and PULL from Unreal Engine. Anchorpoint does handle Git and Git LFS in an optimized way
		and I don't want to replicate that behaviour for each plugin. Thus, I think it's best for Unreal Engine to tell Anchorpoint 
		via IPC to execute these operations.
		Nice benefit is, that if the user pulls in Unreal Engine, Anchorpoint will show so as well.

	3)	We implement our own locking mechanism that's superior to git LFS native locking. Use the apsync library to show existing locks
		in the UI but also to automatically lock files as soon as they are modified (before they are saved)

**/

class FAnchorpointLogger;

class FAnchorpointModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FAnchorpointModule& Get();

	FAnchorpointSourceControlProvider& GetProvider();

private:
	void ApiExample() const;

	std::shared_ptr<apsync::Api> _Api = nullptr;
	USubscription* _SubscriptionHandler;

	FAnchorpointSourceControlProvider AnchorpointSourceControlProvider;
	std::shared_ptr<FAnchorpointLogger> Logger;
};
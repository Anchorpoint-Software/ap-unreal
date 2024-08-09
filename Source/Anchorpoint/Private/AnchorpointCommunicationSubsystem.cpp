// Some copyright should be here...

#include "AnchorpointCommunicationSubsystem.h"

#include "AnchorpointLog.h"
#include "AssetViewUtils.h"
#include "ISourceControlModule.h"
#include "PackageTools.h"
#include "SourceControlHelpers.h"
#include "SourceControlOperations.h"
#include "apsync/service/api.h"

static FAutoConsoleCommand CVarAnchorpointUnlinkPackages(
	TEXT("g.anchorpoint.packageaction"),
	TEXT("Unlinks packages to allow syncs"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() != 2)
		{
			return;
		}

		FString Command = Args[0];
		TArray<FString> PackageNames = {Args[1]};

		if (Command == TEXT("unlink"))
		{
			GEditor->GetEditorSubsystem<UAnchorpointCommunicationSubsystem>()->UnlinkPackages(PackageNames);
		}
		else if (Command == TEXT("relink"))
		{
			GEditor->GetEditorSubsystem<UAnchorpointCommunicationSubsystem>()->RelinkPackages(PackageNames);
		}
	}));

bool UAnchorpointCommunicationSubsystem::OpenDesktopApp(bool bMinimized /* = true */)
{
	return true;
	const apsync::Result<void> Result = apsync::Api::startAnchorpoint(std::nullopt, bMinimized);
	if (Result.has_error())
	{
		UE_LOG(LogAnchorpoint, Error, TEXT("Failed to open Anchorpoint: %s"), *FString(Result.error().message().c_str()));
		return false;
	}

	apsync::Result<std::shared_ptr<apsync::Api>> ApiResult = apsync::Api::createFromAuthenticatedUser(TCHAR_TO_UTF8(*IpcSenderId));
	if (ApiResult.has_error())
	{
		UE_LOG(LogAnchorpoint, Error, TEXT("Failed to create API: %s"), *FString(ApiResult.error().message().c_str()));
		return false;
	}

	UE_LOG(LogAnchorpoint, Verbose, TEXT("Successfully connected to Desktop App"));
	return ApiResult.value().get() != nullptr;
}

UWorld* UAnchorpointCommunicationSubsystem::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

ETickableTickType UAnchorpointCommunicationSubsystem::GetTickableTickType() const
{
	return ETickableTickType::Always;
}

bool UAnchorpointCommunicationSubsystem::IsTickableInEditor() const
{
	return true;
}

bool UAnchorpointCommunicationSubsystem::IsAllowedToTick() const
{
	return true;
}

void UAnchorpointCommunicationSubsystem::Tick(float DeltaTime)
{
}

TStatId UAnchorpointCommunicationSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAnchorpointCommunicationSubsystem, STATGROUP_Tickables);
}


TArray<UPackage*> LoadedPackages;

void UAnchorpointCommunicationSubsystem::UnlinkPackages(const TArray<FString>& PackageNamesToSync)
{
	// Broadcast sync starting...
	AssetViewUtils::OnSyncStart().Broadcast();

	//TODO: Check if we want this with AP team
	/*
	// Warn about any packages that are being synced without also getting the newest version of their dependencies...
	TArray<FString> OutOfDateDependencies;
	GetOutOfDatePackageDependencies(PackageNamesToSync, OutOfDateDependencies);

	TArray<FString> ExtraPackagesToSync;
	ShowSyncDependenciesDialog(OutOfDateDependencies, ExtraPackagesToSync);

	PackageNamesToSync.Append(ExtraPackagesToSync);
	*/

	const TArray<FString> PackageFilenames = SourceControlHelpers::PackageFilenames(PackageNamesToSync);

	// Form a list of loaded packages to reload...
	TArray<UObject*> LoadedObjects;
	/*TODO: This was removed so it can be shared between methods: TArray<UPackage*> LoadedPackages;*/
	TArray<UPackage*> PendingPackages;
	LoadedObjects.Reserve(PackageNamesToSync.Num());
	LoadedPackages.Reserve(PackageNamesToSync.Num());
	PendingPackages.Reserve(PackageNamesToSync.Num());
	for (const FString& PackageName : PackageNamesToSync)
	{
		UPackage* Package = FindPackage(nullptr, *PackageName);
		if (Package)
		{
			LoadedObjects.Emplace(Package);
			LoadedPackages.Emplace(Package);

			if (!Package->IsFullyLoaded())
			{
				PendingPackages.Emplace(Package);
			}
		}
	}

	// Detach the linkers of any loaded packages so that SCC can overwrite the files...
	if (PendingPackages.Num() > 0)
	{
		FlushAsyncLoading();

		for (UPackage* Package : PendingPackages)
		{
			Package->FullyLoad();
		}
	}
	if (LoadedObjects.Num() > 0)
	{
		ResetLoaders(LoadedObjects);
	}
}

void UAnchorpointCommunicationSubsystem::RelinkPackages(const TArray<FString>& PackageNamesToSync)
{
	const TArray<FString> PackageFilenames = SourceControlHelpers::PackageFilenames(PackageNamesToSync);

	// Syncing may have deleted some packages, so we need to unload those rather than re-load them...
	// Note: we will store the package using weak pointers here otherwise we might have garbage collection issues after the ReloadPackages call
	TArray<TWeakObjectPtr<UPackage>> PackagesToUnload;
	LoadedPackages.RemoveAll([&](UPackage* InPackage) -> bool
	{
		const FString PackageExtension = InPackage->ContainsMap() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension();
		const FString PackageFilename = FPackageName::LongPackageNameToFilename(InPackage->GetName(), PackageExtension);
		if (!FPaths::FileExists(PackageFilename))
		{
			PackagesToUnload.Emplace(MakeWeakObjectPtr(InPackage));
			return true; // remove package
		}
		return false; // keep package
	});

	// Hot-reload the new packages...
	UPackageTools::ReloadPackages(LoadedPackages);

	// Unload any deleted packages...
	TArray<UPackage*> PackageRawPtrsToUnload;
	for (TWeakObjectPtr<UPackage>& PackageToUnload : PackagesToUnload)
	{
		if (PackageToUnload.IsValid())
		{
			PackageRawPtrsToUnload.Emplace(PackageToUnload.Get());
		}
	}

	UPackageTools::UnloadPackages(PackageRawPtrsToUnload);

	// Re-cache the SCC state...
	ISourceControlProvider& SCCProvider = ISourceControlModule::Get().GetProvider();
	SCCProvider.Execute(ISourceControlOperation::Create<FUpdateStatus>(), PackageFilenames);

	// Broadcast sync finished...
	AssetViewUtils::OnSyncFinish().Broadcast(true, &PackageFilenames);

	LoadedPackages.Empty();
}
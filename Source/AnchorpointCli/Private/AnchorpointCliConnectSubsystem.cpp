// Fill out your copyright notice in the Description page of Project Settings.

#include "AnchorpointCliConnectSubsystem.h"

#include <ISourceControlModule.h>
#include <ISourceControlProvider.h>
#include <JsonObjectConverter.h>
#include <SourceControlOperations.h>
#include <Editor/EditorPerformanceSettings.h>
#include <Misc/InteractiveProcess.h>
#include <PackageTools.h>
#include <SourceControlHelpers.h>
#include <UnsavedAssetsTrackerModule.h>
#include <UObject/Package.h>

#include "AnchorpointCli.h"

FString GetCliPath()
{
	const FString CliDirectory = FAnchorpointCliModule::Get().GetCliPath();

#if PLATFORM_WINDOWS
	return CliDirectory / "ap.exe";
#elif PLATFORM_MAC
	return CliDirectory / "ap";
#endif
}

DEFINE_LOG_CATEGORY_STATIC(LogAnchorpointCliConnect, Log, All);

void UAnchorpointCliConnectSubsystem::RespondToMessage(const FString& Id, TOptional<FString> Error)
{
	const FString ErrorString = Error.Get(TEXT(""));
	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Responding to message %s with error %s"), *Id, *ErrorString);

	FAnchorpointCliConnectResponse Response;
	Response.Id = Id;
	Response.Error = ErrorString;

	FString JsonMessage;
	FJsonObjectConverter::UStructToJsonObjectString<FAnchorpointCliConnectResponse>(Response, JsonMessage, 0, 0, 0, nullptr, false);

	Process->SendWhenReady(JsonMessage);
}

bool UAnchorpointCliConnectSubsystem::UnlinkObjects(const TArray<FString>& InFiles)
{
	if (!UnlinkedObjects.IsEmpty())
	{
		// We already unlinked something, so we are in the middle of a sync
		return false;
	}

	FlushAsyncLoading();

	const TArray<FString> PackageFilenames = SourceControlHelpers::PackageFilenames(InFiles);
	for (const FString& PackageName : PackageFilenames)
	{
		if (UPackage* Package = FindPackage(nullptr, *PackageName))
		{
			UnlinkedObjects.Emplace(Package);

			if (!Package->IsFullyLoaded())
			{
				Package->FullyLoad();
			}
		}
	}

	TArray<UObject*> Loaders;
	Loaders.Append(UnlinkedObjects);
	ResetLoaders(Loaders);

	return true;
}

bool UAnchorpointCliConnectSubsystem::RelinkObjects(const TArray<FString>& InFiles)
{
	if (UnlinkedObjects.IsEmpty())
	{
		// We have not unlinked anything yet
		return false;
	}

	const TArray<FString> PackageFilenames = SourceControlHelpers::PackageFilenames(InFiles);

	// Syncing may have deleted some packages, so we need to unload those rather than re-load them...
	// Note: we will store the package using weak pointers here otherwise we might have garbage collection issues after the ReloadPackages call
	TArray<TWeakObjectPtr<UPackage>> PackagesToUnload;
	UnlinkedObjects.RemoveAll([&](UPackage* InPackage) -> bool
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
	UPackageTools::ReloadPackages(UnlinkedObjects);

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

	UnlinkedObjects.Empty();

	return true;
}

void UAnchorpointCliConnectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UEditorPerformanceSettings* EditorPerformanceSettings = GetMutableDefault<UEditorPerformanceSettings>())
	{
		EditorPerformanceSettings->bThrottleCPUWhenNotForeground = false;
		EditorPerformanceSettings->PostEditChange();
	}

	const FString CommandLineExecutable = GetCliPath();

	TArray<FString> Args;
	Args.Add(FString::Printf(TEXT("--cwd=\"%s\""), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())));
	Args.Add(TEXT("connect"));
	Args.Add(TEXT("--name \"Unreal\""));
	Args.Add(TEXT("--projectSaved"));
	Args.Add(TEXT("--changedFiles"));
	const FString CommandLineArgs = FString::Join(Args, TEXT(" "));

	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Running %s %s"), *CommandLineExecutable, *CommandLineArgs);
	Process = MakeShared<FInteractiveProcess>(CommandLineExecutable, CommandLineArgs, true, true);

	if (!Process)
	{
		UE_LOG(LogAnchorpointCliConnect, Error, TEXT("Failed to create process"));
		return;
	}

	const bool bLaunchSuccess = Process->Launch();
	if (!bLaunchSuccess)
	{
		UE_LOG(LogAnchorpointCliConnect, Error, TEXT("Failed to launch process"));
		return;
	}

	Process->OnOutput().BindUObject(this, &UAnchorpointCliConnectSubsystem::OnOutput);
	Process->OnCompleted().BindUObject(this, &UAnchorpointCliConnectSubsystem::OnCompleted);
	Process->OnCanceled().BindUObject(this, &UAnchorpointCliConnectSubsystem::OnCanceled);

	UE_LOG(LogAnchorpointCliConnect, Display, TEXT("Anchopoint listener connected"));
}

void UAnchorpointCliConnectSubsystem::HandleMessage(const FAnchorpointConnectMessage& Message)
{
	const FString& MessageType = Message.Type;
	UE_LOG(LogAnchorpointCliConnect, Display, TEXT("Anchorpoint Source Control Provider received message: %s"), *MessageType);

	if (MessageType == TEXT("files locked") || MessageType == TEXT("files unlocked") || MessageType == TEXT("files outdated"))
	{
		AsyncTask(ENamedThreads::GameThread,
		          [this, Message]()
		          {
			          UE_LOG(LogAnchorpointCliConnect, Display, TEXT("Update Status requested by Cli Connect"));
			          ISourceControlModule::Get().GetProvider().Execute(ISourceControlOperation::Create<FUpdateStatus>(), Message.Files, EConcurrency::Asynchronous);
		          });
	}
	else if (MessageType == TEXT("project saved"))
	{
		TOptional<FString> Error;
		const int32 NumUnsaved = FUnsavedAssetsTrackerModule::Get().GetUnsavedAssets().Num();
		if (NumUnsaved > 0)
		{
			Error = FString::Printf(TEXT("There are %d unsaved assets"), NumUnsaved);
		}

		RespondToMessage(Message.Id, Error);
	}
	else if (MessageType == TEXT("files about to change"))
	{
		UnlinkObjects(Message.Files);
		RespondToMessage(Message.Id);
	}
	else if (MessageType == TEXT("files changed"))
	{
		// RespondToMessage(MessageId);
	}
}

void UAnchorpointCliConnectSubsystem::OnOutput(const FString& Output)
{
	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Listener output: %s"), *Output);

	FAnchorpointConnectMessage Message;

	if (FJsonObjectConverter::JsonObjectStringToUStruct(Output, &Message))
	{
		// Anchorpoint CLI sends relative paths, so we need to convert them to full paths
		const FString ProjectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
		for (FString& File : Message.Files)
		{
			File = ProjectPath / File;
		}

		HandleMessage(Message);
	}
	else
	{
		UE_LOG(LogAnchorpointCliConnect, Error, TEXT("Failed to parse message: %s"), *Output);
	}
}

void UAnchorpointCliConnectSubsystem::OnCompleted(int ReturnCode, bool bCanceling)
{
	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Listener completed with exit code %d Cannceling: %s"), ReturnCode, *LexToString(bCanceling));
}

void UAnchorpointCliConnectSubsystem::OnCanceled()
{
	UE_LOG(LogAnchorpointCliConnect, Verbose, TEXT("Listener canceled"));
}
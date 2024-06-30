// Some copyright should be here...

#include "AnchorpointCommunicationSubsystem.h"

#include "apsync/service/api.h"

bool UAnchorpointCommunicationSubsystem::OpenDesktopApp(bool bMinimized /* = true */)
{
	const apsync::Result<void> Result = apsync::Api::startAnchorpoint(std::nullopt, bMinimized);
	if (Result.has_error())
	{
		return false;
	}

	apsync::Result<std::shared_ptr<apsync::Api>> ApiResult = apsync::Api::createFromAuthenticatedUser(TCHAR_TO_UTF8(*IpcSenderId));
	if (ApiResult.has_error())
	{
		const std::string Error = ApiResult.error().message();
		//TODO: Replace with Anchorpoint Log category
		UE_LOG(LogTemp, Error, TEXT("Failed to create API: %s"), *FString(Error.c_str()));
		return false;
	}

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
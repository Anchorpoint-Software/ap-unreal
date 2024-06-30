// Some copyright should be here...

#include "AnchorpointLogger.h"

#include "AnchorpointLog.h"

void FAnchorpointLogger::log(apsync::LogLevel level, std::string& msg)
{
	const FString& LogMessage = UTF8_TO_TCHAR(msg.c_str());

	switch (level)
	{
	case apsync::LogLevel::Critical:
		UE_LOG(LogAnchorpoint, Error, TEXT("[AP-SYNC] %s"), *LogMessage);
		break;
	case apsync::LogLevel::Warning:
		UE_LOG(LogAnchorpoint, Warning, TEXT("[AP-SYNC] %s"), *LogMessage);
		break;
	case apsync::LogLevel::Info:
		UE_LOG(LogAnchorpoint, Display, TEXT("[AP-SYNC] %s"), *LogMessage);
		break;
	case apsync::LogLevel::Debug:
		UE_LOG(LogAnchorpoint, Verbose, TEXT("[AP-SYNC] %s"), *LogMessage);
		break;
	default:
		UE_LOG(LogAnchorpoint, Log, TEXT("[AP-SYNC] %s"), *LogMessage);
		break;
	}
}

apsync::LogHandle FAnchorpointLogger::getLogStream(apsync::LogLevel level)
{
	//TODO: What is this suppossed to do?
	return apsync::LogHandle({});
}
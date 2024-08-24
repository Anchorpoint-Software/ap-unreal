// Some copyright should be here...

#include "AnchorpointLogger.h"

#include "Subscription.h"

namespace
{
	void LogString(apsync::LogLevel InLevel, FString InMessage)
	{
		switch (InLevel)
		{
		case apsync::LogLevel::Critical:
			UE_LOG(LogAnchorpointSync, Error, TEXT("[AP-SYNC] %s"), *InMessage);
			break;
		case apsync::LogLevel::Warning:
			UE_LOG(LogAnchorpointSync, Warning, TEXT("[AP-SYNC] %s"), *InMessage);
			break;
		case apsync::LogLevel::Info:
			UE_LOG(LogAnchorpointSync, Display, TEXT("[AP-SYNC] %s"), *InMessage);
			break;
		case apsync::LogLevel::Debug:
			UE_LOG(LogAnchorpointSync, Verbose, TEXT("[AP-SYNC] %s"), *InMessage);
			break;
		default:
			UE_LOG(LogAnchorpointSync, Log, TEXT("[AP-SYNC] %s"), *InMessage);
			break;
		}
	}
}

FAnchorpointLog::FAnchorpointLog(apsync::LogLevel InLevel) : ILog()
{
	LogLevel = InLevel;
}

void FAnchorpointLog::appendLog(const std::string& data)
{
	const FString& DataString = UTF8_TO_TCHAR(data.c_str());

	LogString(LogLevel, DataString);
}

void FAnchorpointLog::endline()
{
	LogString(LogLevel, LINE_TERMINATOR);
}

void FAnchorpointLogger::log(apsync::LogLevel level, std::string& msg)
{
	const FString& LogMessage = UTF8_TO_TCHAR(msg.c_str());

	LogString(level, LogMessage);
}

apsync::LogHandle FAnchorpointLogger::getLogStream(apsync::LogLevel level)
{
	return apsync::LogHandle(std::make_shared<FAnchorpointLog>(level));
}
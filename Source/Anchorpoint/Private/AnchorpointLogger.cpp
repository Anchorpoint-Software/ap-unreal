// Some copyright should be here...

#include "AnchorpointLogger.h"

#include "AnchorpointLog.h"

namespace
{
	void LogString(apsync::LogLevel InLevel, FString InMessage)
	{
		switch (InLevel)
		{
		case apsync::LogLevel::Critical:
			UE_LOG(LogAnchorpoint, Error, TEXT("[AP-SYNC] %s"), *InMessage);
			break;
		case apsync::LogLevel::Warning:
			UE_LOG(LogAnchorpoint, Warning, TEXT("[AP-SYNC] %s"), *InMessage);
			break;
		case apsync::LogLevel::Info:
			UE_LOG(LogAnchorpoint, Display, TEXT("[AP-SYNC] %s"), *InMessage);
			break;
		case apsync::LogLevel::Debug:
			UE_LOG(LogAnchorpoint, Verbose, TEXT("[AP-SYNC] %s"), *InMessage);
			break;
		default:
			UE_LOG(LogAnchorpoint, Log, TEXT("[AP-SYNC] %s"), *InMessage);
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
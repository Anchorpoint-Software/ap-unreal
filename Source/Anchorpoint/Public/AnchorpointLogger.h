// Some copyright should be here...

#pragma once

#include "apsync/service/logger.h"

/**
 * Implementation of the apsync::ILogger responsible for forwarding logs to Unreal's system 
 */
class FAnchorpointLogger : public apsync::ILogger
{
public:
	virtual ~FAnchorpointLogger() = default;

private:
	// Begin apsync::ILogger interface
	virtual void log(apsync::LogLevel level, std::string& msg) override;
	virtual apsync::LogHandle getLogStream(apsync::LogLevel level) override;
	// End apsync::ILogger interface
};
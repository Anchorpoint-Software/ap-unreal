// Some copyright should be here...

#pragma once

#include "apsync/service/logger.h"

class ANCHORPOINT_API FAnchorpointLog final : public apsync::ILog
{
public:
	explicit FAnchorpointLog(apsync::LogLevel InLevel);

private:
	// Begin apsync::ILog interface
	virtual void appendLog(const std::string& data) override;
	virtual void endline() override;
	// End apsync::ILog interface

	apsync::LogLevel LogLevel = apsync::LogLevel::Debug;
};

/**
 * Implementation of the apsync::ILogger responsible for forwarding logs to Unreal's system 
 */
class ANCHORPOINT_API FAnchorpointLogger final : public apsync::ILogger
{
public:
	virtual ~FAnchorpointLogger() = default;

private:
	// Begin apsync::ILogger interface
	virtual void log(apsync::LogLevel level, std::string& msg) override;
	virtual apsync::LogHandle getLogStream(apsync::LogLevel level) override;
	// End apsync::ILogger interface
};
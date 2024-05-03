#ifndef LOGGER_H
#define LOGGER_H

#include "apsync/sync_global.h"

namespace apsync {

struct Endl {
};

enum class LogLevel { Debug = 0, Info, Warning, Critical };

class SYNC_EXPORT ILog
{
public:
    virtual void appendLog(const std::string& data) = 0;
    virtual void endline() = 0;
};

class SYNC_EXPORT LogHandle
{
public:
    LogHandle(std::shared_ptr<ILog> log) : _log(log) {}

    LogHandle& operator<<(const std::string& data)
    {
        _log->appendLog(data);
        return *this;
    }

    LogHandle& operator<<(Endl)
    {
        _log->endline();
        return *this;
    }

private:
    std::shared_ptr<ILog> _log;
};

class SYNC_EXPORT ILogger
{
public:
    virtual void log(LogLevel level, std::string& msg) = 0;
    virtual LogHandle getLogStream(LogLevel level) = 0;
};

class DefaultLogger: public ILogger
{
public:
    DefaultLogger(LogLevel maxLevel);

    void log(LogLevel level, std::string& msg) override;
    LogHandle getLogStream(LogLevel level) override;

private:
    LogLevel _maxLevel;
};

class DefaultLog: public ILog
{
public:
    DefaultLog(LogLevel maxLevel, LogLevel level, std::ostream& stream = std::cout)
        : _outStream(stream),
          _maxLevel(maxLevel),
          _level(level)
    {}

    void appendLog(const std::string& data) override;
    void endline() override;

private:
    std::ostream& _outStream;
    LogLevel _maxLevel;
    LogLevel _level;
};

class SYNC_EXPORT Logger
{
public:
    static Logger& instance();

    void setLogger(std::shared_ptr<ILogger> logger);

    LogHandle debug();
    LogHandle info();
    LogHandle warning();
    LogHandle critical();

    void debug(std::string msg);
    void info(std::string msg);
    void warning(std::string msg);
    void critical(std::string msg);

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

    std::shared_ptr<ILogger> _logger;
};

}; // namespace apsync

#endif // LOGGER_H

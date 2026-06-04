// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include <QObject>
#include <QFile>
#include <QMutex>
#include <QDateTime>

#include <memory>

namespace aips::utils {

class Logger : public QObject {
    Q_OBJECT

public:
    enum class Level : uint8_t {
        Trace = 0,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };
    Q_ENUM(Level)

    static Logger& instance();

    void init(const QString& logFilePath = {}, Level minLevel = Level::Info);
    void shutdown();

    void log(Level level, const QString& category, const QString& message);

    void trace(const QString& category, const QString& msg)   { log(Level::Trace, category, msg); }
    void debug(const QString& category, const QString& msg)   { log(Level::Debug, category, msg); }
    void info(const QString& category, const QString& msg)    { log(Level::Info, category, msg); }
    void warning(const QString& category, const QString& msg) { log(Level::Warning, category, msg); }
    void error(const QString& category, const QString& msg)   { log(Level::Error, category, msg); }
    void fatal(const QString& category, const QString& msg)   { log(Level::Fatal, category, msg); }

    void setMinLevel(Level level) { m_minLevel = level; }
    void setConsoleOutput(bool enabled) { m_consoleOutput = enabled; }
    void setMaxFileSize(qint64 bytes) { m_maxFileSize = bytes; }

signals:
    void logEntry(Level level, const QString& category, const QString& message);

private:
    Logger();
    ~Logger() override;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void rotateLogFile();
    [[nodiscard]] static QString levelToString(Level level);

    QMutex          m_mutex;
    std::unique_ptr<QFile> m_logFile;
    Level           m_minLevel      = Level::Info;
    bool            m_consoleOutput = true;
    bool            m_initialized   = false;
    qint64          m_maxFileSize   = 10 * 1024 * 1024; // 10 MB
};

} // namespace aips::utils

#define AIPS_LOG_TRACE(cat, msg)   aips::utils::Logger::instance().trace(cat, msg)
#define AIPS_LOG_DEBUG(cat, msg)   aips::utils::Logger::instance().debug(cat, msg)
#define AIPS_LOG_INFO(cat, msg)    aips::utils::Logger::instance().info(cat, msg)
#define AIPS_LOG_WARNING(cat, msg) aips::utils::Logger::instance().warning(cat, msg)
#define AIPS_LOG_ERROR(cat, msg)   aips::utils::Logger::instance().error(cat, msg)

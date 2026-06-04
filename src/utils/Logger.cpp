// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "utils/Logger.h"

#include <QTextStream>
#include <QDir>
#include <QFileInfo>

#include <iostream>

namespace aips::utils {

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

Logger::Logger() = default;

Logger::~Logger()
{
    shutdown();
}

void Logger::init(const QString& logFilePath, Level minLevel)
{
    QMutexLocker lock(&m_mutex);

    m_minLevel = minLevel;

    if (!logFilePath.isEmpty()) {
        QDir dir = QFileInfo(logFilePath).absoluteDir();
        if (!dir.exists())
            dir.mkpath(".");

        m_logFile = std::make_unique<QFile>(logFilePath);
        if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            std::cerr << "Logger: failed to open log file: "
                      << logFilePath.toStdString() << std::endl;
            m_logFile.reset();
        }
    }

    m_initialized = true;
}

void Logger::shutdown()
{
    QMutexLocker lock(&m_mutex);
    if (m_logFile) {
        m_logFile->flush();
        m_logFile->close();
        m_logFile.reset();
    }
    m_initialized = false;
}

void Logger::log(Level level, const QString& category, const QString& message)
{
    if (level < m_minLevel)
        return;

    QMutexLocker lock(&m_mutex);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr  = levelToString(level);
    QString line      = QString("[%1] [%2] [%3] %4")
                            .arg(timestamp, levelStr, category, message);

    if (m_consoleOutput) {
        QTextStream stream(level >= Level::Warning ? stderr : stdout);
        stream << line << "\n";
        stream.flush();
    }

    if (m_logFile && m_logFile->isOpen()) {
        QTextStream stream(m_logFile.get());
        stream << line << "\n";
        stream.flush();

        if (m_logFile->size() > m_maxFileSize)
            rotateLogFile();
    }

    emit logEntry(level, category, message);
}

void Logger::rotateLogFile()
{
    if (!m_logFile)
        return;

    QString path = m_logFile->fileName();
    m_logFile->close();

    QString backupPath = path + ".1";
    QFile::remove(backupPath);
    QFile::rename(path, backupPath);

    m_logFile->setFileName(path);
    m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

QString Logger::levelToString(Level level)
{
    switch (level) {
        case Level::Trace:   return QStringLiteral("TRACE");
        case Level::Debug:   return QStringLiteral("DEBUG");
        case Level::Info:    return QStringLiteral("INFO ");
        case Level::Warning: return QStringLiteral("WARN ");
        case Level::Error:   return QStringLiteral("ERROR");
        case Level::Fatal:   return QStringLiteral("FATAL");
    }
    return QStringLiteral("?????");
}

} // namespace aips::utils

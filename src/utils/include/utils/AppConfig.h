// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/ScanOptions.h"

#include <QSettings>
#include <QString>

#include <memory>

namespace aips::utils {

class AppConfig {
public:
    static AppConfig& instance();

    void load();
    void save();
    void resetToDefaults();

    [[nodiscard]] QString                   language()     const { return m_language; }
    [[nodiscard]] bool                      minimizeToTray() const { return m_minimizeToTray; }
    [[nodiscard]] bool                      startMinimized() const { return m_startMinimized; }
    [[nodiscard]] bool                      checkUpdates() const { return m_checkUpdates; }
    [[nodiscard]] QString                   lastRange()    const { return m_lastRange; }
    [[nodiscard]] aips::core::ScanOptions   scanOptions()  const { return m_scanOptions; }

    void setLanguage(const QString& lang)              { m_language = lang; }
    void setMinimizeToTray(bool enabled)               { m_minimizeToTray = enabled; }
    void setStartMinimized(bool enabled)               { m_startMinimized = enabled; }
    void setCheckUpdates(bool enabled)                 { m_checkUpdates = enabled; }
    void setLastRange(const QString& range)            { m_lastRange = range; }
    void setScanOptions(const aips::core::ScanOptions& opts) { m_scanOptions = opts; }

    [[nodiscard]] QString configFilePath() const;
    [[nodiscard]] QString dataDir() const;
    [[nodiscard]] QString favoritesFilePath() const;
    [[nodiscard]] bool isPortable() const;

private:
    AppConfig();
    ~AppConfig() = default;
    AppConfig(const AppConfig&) = delete;
    AppConfig& operator=(const AppConfig&) = delete;

    std::unique_ptr<QSettings> createSettings() const;

    QString                   m_language       = "en";
    bool                      m_minimizeToTray = false;
    bool                      m_startMinimized = false;
    bool                      m_checkUpdates   = true;
    QString                   m_lastRange      = "192.168.1.1-254";
    aips::core::ScanOptions   m_scanOptions;
};

} // namespace aips::utils

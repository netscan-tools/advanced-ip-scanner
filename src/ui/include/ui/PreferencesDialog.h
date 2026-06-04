// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/ScanOptions.h"

#include <QDialog>
#include <QTabWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QComboBox>
#include <QGroupBox>

namespace aips::ui {

class PreferencesDialog : public QDialog {
    Q_OBJECT

public:
    explicit PreferencesDialog(const aips::core::ScanOptions& currentOptions,
                                QWidget* parent = nullptr);
    ~PreferencesDialog() override = default;

    [[nodiscard]] aips::core::ScanOptions options() const;

private slots:
    void onPresetChanged(int index);
    void onApply();
    void onReset();

private:
    void setupUi();
    void loadOptions(const aips::core::ScanOptions& opt);
    QWidget* createGeneralTab();
    QWidget* createScanTab();
    QWidget* createPortsTab();
    QWidget* createResolutionTab();
    QWidget* createPerformanceTab();

    QTabWidget*  m_tabs = nullptr;

    // General
    QComboBox*   m_languageCombo  = nullptr;
    QCheckBox*   m_minimizeToTray = nullptr;
    QCheckBox*   m_startMinimized = nullptr;
    QCheckBox*   m_autoUpdate     = nullptr;

    // Scan
    QCheckBox*   m_useIcmp        = nullptr;
    QCheckBox*   m_useArp         = nullptr;
    QSpinBox*    m_icmpTimeout    = nullptr;
    QSpinBox*    m_icmpRetries    = nullptr;
    QSpinBox*    m_arpTimeout     = nullptr;
    QCheckBox*   m_skipOffline    = nullptr;

    // Ports
    QCheckBox*   m_scanCommonPorts = nullptr;
    QCheckBox*   m_scanAllPorts    = nullptr;
    QSpinBox*    m_portTimeout     = nullptr;
    QSpinBox*    m_portConcurrency = nullptr;

    // Resolution
    QCheckBox*   m_resolveHostnames = nullptr;
    QCheckBox*   m_resolveNetbios   = nullptr;
    QCheckBox*   m_detectVendor     = nullptr;
    QCheckBox*   m_detectOs         = nullptr;
    QCheckBox*   m_enumShares       = nullptr;
    QSpinBox*    m_dnsTimeout       = nullptr;
    QSpinBox*    m_netbiosTimeout   = nullptr;

    // Performance
    QSlider*     m_speedSlider      = nullptr;
    QSpinBox*    m_threadCount      = nullptr;
    QComboBox*   m_presetCombo      = nullptr;

    aips::core::ScanOptions m_originalOptions;
};

} // namespace aips::ui

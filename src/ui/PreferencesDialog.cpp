// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "ui/PreferencesDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>

namespace aips::ui {

PreferencesDialog::PreferencesDialog(const aips::core::ScanOptions& currentOptions,
                                      QWidget* parent)
    : QDialog(parent)
    , m_originalOptions(currentOptions)
{
    setWindowTitle(tr("Preferences"));
    setMinimumSize(500, 450);
    setupUi();
    loadOptions(currentOptions);
}

aips::core::ScanOptions PreferencesDialog::options() const
{
    aips::core::ScanOptions opt;

    opt.threadCount        = m_threadCount->value();
    opt.icmpTimeoutMs      = m_icmpTimeout->value();
    opt.icmpRetries        = m_icmpRetries->value();
    opt.arpTimeoutMs       = m_arpTimeout->value();
    opt.dnsTimeoutMs       = m_dnsTimeout->value();
    opt.netbiosTimeoutMs   = m_netbiosTimeout->value();
    opt.scanSpeedPercent   = m_speedSlider->value();
    opt.useIcmp            = m_useIcmp->isChecked();
    opt.useArp             = m_useArp->isChecked();
    opt.resolveHostnames   = m_resolveHostnames->isChecked();
    opt.resolveNetbios     = m_resolveNetbios->isChecked();
    opt.detectVendor       = m_detectVendor->isChecked();
    opt.detectOs           = m_detectOs->isChecked();
    opt.enumerateShares    = m_enumShares->isChecked();
    opt.skipOfflineDevices = m_skipOffline->isChecked();

    opt.portScan.scanCommonPorts  = m_scanCommonPorts->isChecked();
    opt.portScan.scanAllPorts     = m_scanAllPorts->isChecked();
    opt.portScan.connectTimeoutMs = m_portTimeout->value();
    opt.portScan.maxConcurrent    = m_portConcurrency->value();

    return opt;
}

void PreferencesDialog::setupUi()
{
    auto* layout = new QVBoxLayout(this);

    // Preset selector
    auto* presetRow = new QHBoxLayout();
    presetRow->addWidget(new QLabel(tr("Preset:"), this));
    m_presetCombo = new QComboBox(this);
    m_presetCombo->addItem(tr("Default"));
    m_presetCombo->addItem(tr("Fast Scan"));
    m_presetCombo->addItem(tr("Deep Scan"));
    m_presetCombo->addItem(tr("Custom"));
    presetRow->addWidget(m_presetCombo, 1);
    layout->addLayout(presetRow);

    m_tabs = new QTabWidget(this);
    m_tabs->addTab(createGeneralTab(),     tr("General"));
    m_tabs->addTab(createScanTab(),        tr("Scan"));
    m_tabs->addTab(createPortsTab(),       tr("Ports"));
    m_tabs->addTab(createResolutionTab(),  tr("Resolution"));
    m_tabs->addTab(createPerformanceTab(), tr("Performance"));
    layout->addWidget(m_tabs, 1);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults,
        this
    );
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons->button(QDialogButtonBox::RestoreDefaults),
            &QPushButton::clicked, this, &PreferencesDialog::onReset);
    layout->addWidget(buttons);

    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PreferencesDialog::onPresetChanged);
}

void PreferencesDialog::loadOptions(const aips::core::ScanOptions& opt)
{
    m_useIcmp->setChecked(opt.useIcmp);
    m_useArp->setChecked(opt.useArp);
    m_icmpTimeout->setValue(opt.icmpTimeoutMs);
    m_icmpRetries->setValue(opt.icmpRetries);
    m_arpTimeout->setValue(opt.arpTimeoutMs);
    m_skipOffline->setChecked(opt.skipOfflineDevices);

    m_resolveHostnames->setChecked(opt.resolveHostnames);
    m_resolveNetbios->setChecked(opt.resolveNetbios);
    m_detectVendor->setChecked(opt.detectVendor);
    m_detectOs->setChecked(opt.detectOs);
    m_enumShares->setChecked(opt.enumerateShares);
    m_dnsTimeout->setValue(opt.dnsTimeoutMs);
    m_netbiosTimeout->setValue(opt.netbiosTimeoutMs);

    m_scanCommonPorts->setChecked(opt.portScan.scanCommonPorts);
    m_scanAllPorts->setChecked(opt.portScan.scanAllPorts);
    m_portTimeout->setValue(opt.portScan.connectTimeoutMs);
    m_portConcurrency->setValue(opt.portScan.maxConcurrent);

    m_speedSlider->setValue(opt.scanSpeedPercent);
    m_threadCount->setValue(opt.threadCount);
}

QWidget* PreferencesDialog::createGeneralTab()
{
    auto* w = new QWidget(this);
    auto* layout = new QFormLayout(w);

    m_languageCombo = new QComboBox(w);
    m_languageCombo->addItems({"English", "Deutsch", "Français", "Español",
                               "Italiano", "Português", "Русский", "中文", "日本語"});
    layout->addRow(tr("Language:"), m_languageCombo);

    m_minimizeToTray = new QCheckBox(tr("Minimize to system tray"), w);
    m_startMinimized = new QCheckBox(tr("Start minimized"), w);
    m_autoUpdate     = new QCheckBox(tr("Check for updates automatically"), w);
    m_autoUpdate->setChecked(true);

    layout->addRow(m_minimizeToTray);
    layout->addRow(m_startMinimized);
    layout->addRow(m_autoUpdate);

    return w;
}

QWidget* PreferencesDialog::createScanTab()
{
    auto* w = new QWidget(this);
    auto* layout = new QFormLayout(w);

    m_useIcmp     = new QCheckBox(tr("Use ICMP ping"), w);
    m_useArp      = new QCheckBox(tr("Use ARP resolution"), w);
    m_icmpTimeout = new QSpinBox(w);
    m_icmpTimeout->setRange(50, 5000);
    m_icmpTimeout->setSuffix(" ms");
    m_icmpRetries = new QSpinBox(w);
    m_icmpRetries->setRange(1, 10);
    m_arpTimeout  = new QSpinBox(w);
    m_arpTimeout->setRange(50, 3000);
    m_arpTimeout->setSuffix(" ms");
    m_skipOffline = new QCheckBox(tr("Skip offline devices"), w);

    layout->addRow(m_useIcmp);
    layout->addRow(m_useArp);
    layout->addRow(tr("ICMP Timeout:"), m_icmpTimeout);
    layout->addRow(tr("ICMP Retries:"), m_icmpRetries);
    layout->addRow(tr("ARP Timeout:"),  m_arpTimeout);
    layout->addRow(m_skipOffline);

    return w;
}

QWidget* PreferencesDialog::createPortsTab()
{
    auto* w = new QWidget(this);
    auto* layout = new QFormLayout(w);

    m_scanCommonPorts = new QCheckBox(tr("Scan common ports"), w);
    m_scanAllPorts    = new QCheckBox(tr("Scan all ports (1-65535)"), w);
    m_portTimeout     = new QSpinBox(w);
    m_portTimeout->setRange(50, 5000);
    m_portTimeout->setSuffix(" ms");
    m_portConcurrency = new QSpinBox(w);
    m_portConcurrency->setRange(1, 1024);

    layout->addRow(m_scanCommonPorts);
    layout->addRow(m_scanAllPorts);
    layout->addRow(tr("Connect Timeout:"), m_portTimeout);
    layout->addRow(tr("Max Concurrent:"),  m_portConcurrency);

    return w;
}

QWidget* PreferencesDialog::createResolutionTab()
{
    auto* w = new QWidget(this);
    auto* layout = new QFormLayout(w);

    m_resolveHostnames = new QCheckBox(tr("Resolve DNS hostnames"), w);
    m_resolveNetbios   = new QCheckBox(tr("Resolve NetBIOS names"), w);
    m_detectVendor     = new QCheckBox(tr("Detect hardware vendor (OUI)"), w);
    m_detectOs         = new QCheckBox(tr("Detect operating system"), w);
    m_enumShares       = new QCheckBox(tr("Enumerate shared resources"), w);
    m_dnsTimeout       = new QSpinBox(w);
    m_dnsTimeout->setRange(100, 10000);
    m_dnsTimeout->setSuffix(" ms");
    m_netbiosTimeout   = new QSpinBox(w);
    m_netbiosTimeout->setRange(100, 10000);
    m_netbiosTimeout->setSuffix(" ms");

    layout->addRow(m_resolveHostnames);
    layout->addRow(m_resolveNetbios);
    layout->addRow(m_detectVendor);
    layout->addRow(m_detectOs);
    layout->addRow(m_enumShares);
    layout->addRow(tr("DNS Timeout:"),     m_dnsTimeout);
    layout->addRow(tr("NetBIOS Timeout:"), m_netbiosTimeout);

    return w;
}

QWidget* PreferencesDialog::createPerformanceTab()
{
    auto* w = new QWidget(this);
    auto* layout = new QFormLayout(w);

    m_speedSlider = new QSlider(Qt::Horizontal, w);
    m_speedSlider->setRange(1, 100);
    m_speedSlider->setTickInterval(10);
    m_speedSlider->setTickPosition(QSlider::TicksBelow);

    m_threadCount = new QSpinBox(w);
    m_threadCount->setRange(0, 512);
    m_threadCount->setSpecialValueText(tr("Auto"));

    layout->addRow(tr("Scan Speed (%):"), m_speedSlider);
    layout->addRow(tr("Thread Count:"),   m_threadCount);

    return w;
}

void PreferencesDialog::onPresetChanged(int index)
{
    switch (index) {
        case 0: loadOptions(aips::core::ScanOptions::defaultOptions()); break;
        case 1: loadOptions(aips::core::ScanOptions::fastScan());       break;
        case 2: loadOptions(aips::core::ScanOptions::deepScan());       break;
        default: break;
    }
}

void PreferencesDialog::onApply()
{
    accept();
}

void PreferencesDialog::onReset()
{
    loadOptions(aips::core::ScanOptions::defaultOptions());
    m_presetCombo->setCurrentIndex(0);
}

} // namespace aips::ui

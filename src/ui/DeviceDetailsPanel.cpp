// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "ui/DeviceDetailsPanel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QScrollArea>

namespace aips::ui {

DeviceDetailsPanel::DeviceDetailsPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void DeviceDetailsPanel::setupUi()
{
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* container = new QWidget(scroll);
    auto* layout    = new QVBoxLayout(container);

    // Device info group
    m_infoGroup = new QGroupBox(tr("Device Information"), container);
    auto* infoLayout = new QFormLayout(m_infoGroup);

    m_ipLabel        = new QLabel("-", m_infoGroup);
    m_macLabel       = new QLabel("-", m_infoGroup);
    m_hostnameLabel  = new QLabel("-", m_infoGroup);
    m_vendorLabel    = new QLabel("-", m_infoGroup);
    m_osLabel        = new QLabel("-", m_infoGroup);
    m_netbiosLabel   = new QLabel("-", m_infoGroup);
    m_workgroupLabel = new QLabel("-", m_infoGroup);
    m_statusLabel    = new QLabel("-", m_infoGroup);
    m_latencyLabel   = new QLabel("-", m_infoGroup);
    m_lastSeenLabel  = new QLabel("-", m_infoGroup);

    infoLayout->addRow(tr("IP Address:"),   m_ipLabel);
    infoLayout->addRow(tr("MAC Address:"),  m_macLabel);
    infoLayout->addRow(tr("Hostname:"),     m_hostnameLabel);
    infoLayout->addRow(tr("Vendor:"),       m_vendorLabel);
    infoLayout->addRow(tr("OS:"),           m_osLabel);
    infoLayout->addRow(tr("NetBIOS:"),      m_netbiosLabel);
    infoLayout->addRow(tr("Workgroup:"),    m_workgroupLabel);
    infoLayout->addRow(tr("Status:"),       m_statusLabel);
    infoLayout->addRow(tr("Latency:"),      m_latencyLabel);
    infoLayout->addRow(tr("Last Seen:"),    m_lastSeenLabel);

    layout->addWidget(m_infoGroup);

    // Open ports group
    m_portsGroup = new QGroupBox(tr("Open Ports"), container);
    auto* portsLayout = new QVBoxLayout(m_portsGroup);
    m_portsList = new QListWidget(m_portsGroup);
    m_portsList->setMaximumHeight(120);
    portsLayout->addWidget(m_portsList);
    layout->addWidget(m_portsGroup);

    // Shared resources group
    m_sharesGroup = new QGroupBox(tr("Shared Resources"), container);
    auto* sharesLayout = new QVBoxLayout(m_sharesGroup);
    m_sharesList = new QListWidget(m_sharesGroup);
    m_sharesList->setMaximumHeight(120);
    sharesLayout->addWidget(m_sharesList);
    layout->addWidget(m_sharesGroup);

    // Comment
    m_commentEdit = new QTextEdit(container);
    m_commentEdit->setPlaceholderText(tr("Add a comment..."));
    m_commentEdit->setMaximumHeight(60);
    layout->addWidget(m_commentEdit);

    // Actions group
    m_actionsGroup = new QGroupBox(tr("Actions"), container);
    auto* actionsLayout = new QVBoxLayout(m_actionsGroup);

    auto* row1 = new QHBoxLayout();
    m_rdpBtn  = new QPushButton(tr("Remote Desktop"), m_actionsGroup);
    m_wolBtn  = new QPushButton(tr("Wake-on-LAN"), m_actionsGroup);
    row1->addWidget(m_rdpBtn);
    row1->addWidget(m_wolBtn);
    actionsLayout->addLayout(row1);

    auto* row2 = new QHBoxLayout();
    m_shutdownBtn = new QPushButton(tr("Shutdown"), m_actionsGroup);
    m_browseBtn   = new QPushButton(tr("Browse Shares"), m_actionsGroup);
    row2->addWidget(m_shutdownBtn);
    row2->addWidget(m_browseBtn);
    actionsLayout->addLayout(row2);

    m_pingBtn = new QPushButton(tr("Ping"), m_actionsGroup);
    actionsLayout->addWidget(m_pingBtn);

    layout->addWidget(m_actionsGroup);
    layout->addStretch();

    scroll->setWidget(container);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scroll);

    connect(m_rdpBtn, &QPushButton::clicked, this, [this]() {
        emit rdpRequested(m_currentDevice);
    });
    connect(m_wolBtn, &QPushButton::clicked, this, [this]() {
        emit wolRequested(m_currentDevice);
    });
    connect(m_shutdownBtn, &QPushButton::clicked, this, [this]() {
        emit shutdownRequested(m_currentDevice);
    });
    connect(m_browseBtn, &QPushButton::clicked, this, [this]() {
        emit browseSharesRequested(m_currentDevice);
    });
    connect(m_pingBtn, &QPushButton::clicked, this, [this]() {
        emit pingRequested(m_currentDevice);
    });
    connect(m_commentEdit, &QTextEdit::textChanged, this, [this]() {
        emit commentChanged(m_currentDevice, m_commentEdit->toPlainText());
    });

    clear();
}

void DeviceDetailsPanel::showDevice(const aips::core::Device& device)
{
    m_currentDevice = device;

    m_ipLabel->setText(device.ipAddress().toString());
    m_macLabel->setText(device.macAddressString());
    m_hostnameLabel->setText(device.hostname().isEmpty() ? "-" : device.hostname());
    m_vendorLabel->setText(device.vendor().isEmpty() ? "-" : device.vendor());
    m_osLabel->setText(device.osFamily().isEmpty() ? "-" : device.osFamily());
    m_netbiosLabel->setText(device.netbiosName().isEmpty() ? "-" : device.netbiosName());
    m_workgroupLabel->setText(device.workgroup().isEmpty() ? "-" : device.workgroup());
    m_statusLabel->setText(device.statusString());
    m_latencyLabel->setText(device.latencyMs() > 0
                            ? QString("%1 ms").arg(device.latencyMs(), 0, 'f', 1)
                            : "-");
    m_lastSeenLabel->setText(device.lastSeen().isValid()
                             ? device.lastSeen().toString("yyyy-MM-dd hh:mm:ss")
                             : "-");

    m_commentEdit->blockSignals(true);
    m_commentEdit->setPlainText(device.comment());
    m_commentEdit->blockSignals(false);

    updatePortsList(device);
    updateSharesList(device);

    setEnabled(true);
}

void DeviceDetailsPanel::clear()
{
    m_ipLabel->setText("-");
    m_macLabel->setText("-");
    m_hostnameLabel->setText("-");
    m_vendorLabel->setText("-");
    m_osLabel->setText("-");
    m_netbiosLabel->setText("-");
    m_workgroupLabel->setText("-");
    m_statusLabel->setText("-");
    m_latencyLabel->setText("-");
    m_lastSeenLabel->setText("-");
    m_portsList->clear();
    m_sharesList->clear();
    m_commentEdit->clear();
    setEnabled(false);
}

void DeviceDetailsPanel::updatePortsList(const aips::core::Device& device)
{
    m_portsList->clear();
    for (uint16_t port : device.openPorts()) {
        m_portsList->addItem(QString("%1").arg(port));
    }
    m_portsGroup->setTitle(tr("Open Ports (%1)").arg(device.openPorts().size()));
}

void DeviceDetailsPanel::updateSharesList(const aips::core::Device& device)
{
    m_sharesList->clear();
    for (const auto& share : device.sharedResources()) {
        m_sharesList->addItem(QString("[%1] %2").arg(
            share.displayName.isEmpty() ? share.path : share.displayName,
            share.path
        ));
    }
    m_sharesGroup->setTitle(tr("Shared Resources (%1)").arg(device.sharedResources().size()));
}

} // namespace aips::ui

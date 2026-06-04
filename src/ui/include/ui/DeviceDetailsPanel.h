// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/Device.h"

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>

namespace aips::ui {

class DeviceDetailsPanel : public QWidget {
    Q_OBJECT

public:
    explicit DeviceDetailsPanel(QWidget* parent = nullptr);
    ~DeviceDetailsPanel() override = default;

    void showDevice(const aips::core::Device& device);
    void clear();

signals:
    void rdpRequested(const aips::core::Device& device);
    void wolRequested(const aips::core::Device& device);
    void shutdownRequested(const aips::core::Device& device);
    void browseSharesRequested(const aips::core::Device& device);
    void pingRequested(const aips::core::Device& device);
    void commentChanged(const aips::core::Device& device, const QString& comment);

private:
    void setupUi();
    void updatePortsList(const aips::core::Device& device);
    void updateSharesList(const aips::core::Device& device);

    // Info labels
    QLabel*      m_ipLabel       = nullptr;
    QLabel*      m_macLabel      = nullptr;
    QLabel*      m_hostnameLabel = nullptr;
    QLabel*      m_vendorLabel   = nullptr;
    QLabel*      m_osLabel       = nullptr;
    QLabel*      m_netbiosLabel  = nullptr;
    QLabel*      m_workgroupLabel = nullptr;
    QLabel*      m_statusLabel   = nullptr;
    QLabel*      m_latencyLabel  = nullptr;
    QLabel*      m_lastSeenLabel = nullptr;

    // Panels
    QGroupBox*   m_infoGroup     = nullptr;
    QGroupBox*   m_portsGroup    = nullptr;
    QGroupBox*   m_sharesGroup   = nullptr;
    QGroupBox*   m_actionsGroup  = nullptr;

    QListWidget* m_portsList     = nullptr;
    QListWidget* m_sharesList    = nullptr;
    QTextEdit*   m_commentEdit   = nullptr;

    // Action buttons
    QPushButton* m_rdpBtn        = nullptr;
    QPushButton* m_wolBtn        = nullptr;
    QPushButton* m_shutdownBtn   = nullptr;
    QPushButton* m_browseBtn     = nullptr;
    QPushButton* m_pingBtn       = nullptr;

    aips::core::Device m_currentDevice;
};

} // namespace aips::ui

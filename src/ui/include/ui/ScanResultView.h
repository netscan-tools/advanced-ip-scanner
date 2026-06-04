// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/Device.h"
#include "core/DeviceStore.h"

#include <QWidget>
#include <QTreeView>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

namespace aips::ui {

class ScanResultView : public QWidget {
    Q_OBJECT

public:
    explicit ScanResultView(std::shared_ptr<aips::core::DeviceStore> store,
                            QWidget* parent = nullptr);
    ~ScanResultView() override = default;

    void setFilterText(const QString& text);
    void setShowOnlineOnly(bool enabled);
    void expandAll();
    void collapseAll();
    void selectDevice(const QHostAddress& address);

    [[nodiscard]] QVector<aips::core::Device> selectedDevices() const;
    [[nodiscard]] int visibleDeviceCount() const;

signals:
    void deviceDoubleClicked(const aips::core::Device& device);
    void deviceContextMenuRequested(const aips::core::Device& device, const QPoint& globalPos);
    void selectionChanged(const aips::core::Device& device);

private slots:
    void onDoubleClicked(const QModelIndex& index);
    void onContextMenu(const QPoint& pos);
    void onFilterChanged();
    void onSelectionChanged();

private:
    void setupUi();
    void setupColumns();

    std::shared_ptr<aips::core::DeviceStore> m_store;
    aips::core::DeviceFilterProxy*           m_proxy  = nullptr;
    QTreeView*                               m_treeView = nullptr;
    QLineEdit*                               m_searchBox = nullptr;
    QComboBox*                               m_statusFilter = nullptr;
    QCheckBox*                               m_onlineOnly = nullptr;
    QLabel*                                  m_countLabel = nullptr;
};

} // namespace aips::ui

// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "ui/ScanResultView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>

namespace aips::ui {

ScanResultView::ScanResultView(std::shared_ptr<aips::core::DeviceStore> store,
                                QWidget* parent)
    : QWidget(parent)
    , m_store(std::move(store))
{
    setupUi();
}

void ScanResultView::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* filterBar = new QHBoxLayout();

    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText(tr("Search devices..."));
    m_searchBox->setClearButtonEnabled(true);

    m_statusFilter = new QComboBox(this);
    m_statusFilter->addItem(tr("All Statuses"), -1);
    m_statusFilter->addItem(tr("Online"),  static_cast<int>(aips::core::DeviceStatus::Online));
    m_statusFilter->addItem(tr("Offline"), static_cast<int>(aips::core::DeviceStatus::Offline));

    m_onlineOnly = new QCheckBox(tr("Online only"), this);

    m_countLabel = new QLabel(tr("0 devices"), this);

    filterBar->addWidget(m_searchBox, 1);
    filterBar->addWidget(m_statusFilter);
    filterBar->addWidget(m_onlineOnly);
    filterBar->addStretch();
    filterBar->addWidget(m_countLabel);

    layout->addLayout(filterBar);

    m_proxy = new aips::core::DeviceFilterProxy(this);
    m_proxy->setSourceModel(m_store.get());
    m_proxy->setDynamicSortFilter(true);

    m_treeView = new QTreeView(this);
    m_treeView->setModel(m_proxy);
    m_treeView->setRootIsDecorated(false);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setSortingEnabled(true);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->sortByColumn(aips::core::DeviceStore::ColIpAddress, Qt::AscendingOrder);

    setupColumns();

    layout->addWidget(m_treeView, 1);

    connect(m_searchBox, &QLineEdit::textChanged, this, &ScanResultView::onFilterChanged);
    connect(m_statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ScanResultView::onFilterChanged);
    connect(m_onlineOnly, &QCheckBox::toggled, this, [this](bool checked) {
        m_proxy->setFavoritesOnly(checked);
    });

    connect(m_treeView, &QTreeView::doubleClicked, this, &ScanResultView::onDoubleClicked);
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &ScanResultView::onContextMenu);

    connect(m_treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [this]() { onSelectionChanged(); });

    connect(m_store.get(), &aips::core::DeviceStore::deviceAdded, this, [this]() {
        m_countLabel->setText(tr("%1 devices").arg(m_proxy->rowCount()));
    });
}

void ScanResultView::setupColumns()
{
    auto* header = m_treeView->header();
    header->setStretchLastSection(true);
    header->setSectionResizeMode(QHeaderView::Interactive);

    m_treeView->setColumnWidth(aips::core::DeviceStore::ColStatus, 70);
    m_treeView->setColumnWidth(aips::core::DeviceStore::ColName, 180);
    m_treeView->setColumnWidth(aips::core::DeviceStore::ColIpAddress, 130);
    m_treeView->setColumnWidth(aips::core::DeviceStore::ColMacAddress, 140);
    m_treeView->setColumnWidth(aips::core::DeviceStore::ColVendor, 150);
    m_treeView->setColumnWidth(aips::core::DeviceStore::ColNetbiosName, 120);
    m_treeView->setColumnWidth(aips::core::DeviceStore::ColLatency, 80);
}

void ScanResultView::setFilterText(const QString& text)
{
    m_searchBox->setText(text);
}

void ScanResultView::setShowOnlineOnly(bool enabled)
{
    m_onlineOnly->setChecked(enabled);
}

void ScanResultView::expandAll()
{
    m_treeView->expandAll();
}

void ScanResultView::collapseAll()
{
    m_treeView->collapseAll();
}

void ScanResultView::selectDevice(const QHostAddress& address)
{
    for (int row = 0; row < m_proxy->rowCount(); ++row) {
        QModelIndex idx = m_proxy->index(row, aips::core::DeviceStore::ColIpAddress);
        if (idx.data().toString() == address.toString()) {
            m_treeView->setCurrentIndex(idx);
            m_treeView->scrollTo(idx);
            return;
        }
    }
}

QVector<aips::core::Device> ScanResultView::selectedDevices() const
{
    QVector<aips::core::Device> devices;
    const auto indexes = m_treeView->selectionModel()->selectedRows();
    for (const auto& idx : indexes) {
        auto device = idx.data(Qt::UserRole).value<aips::core::Device>();
        devices.append(device);
    }
    return devices;
}

int ScanResultView::visibleDeviceCount() const
{
    return m_proxy->rowCount();
}

void ScanResultView::onDoubleClicked(const QModelIndex& index)
{
    auto device = index.data(Qt::UserRole).value<aips::core::Device>();
    emit deviceDoubleClicked(device);
}

void ScanResultView::onContextMenu(const QPoint& pos)
{
    QModelIndex index = m_treeView->indexAt(pos);
    if (!index.isValid())
        return;

    auto device = index.data(Qt::UserRole).value<aips::core::Device>();
    emit deviceContextMenuRequested(device, m_treeView->viewport()->mapToGlobal(pos));
}

void ScanResultView::onFilterChanged()
{
    m_proxy->setSearchText(m_searchBox->text());

    int statusVal = m_statusFilter->currentData().toInt();
    if (statusVal >= 0)
        m_proxy->setStatusFilter(static_cast<aips::core::DeviceStatus>(statusVal));
    else
        m_proxy->clearFilters();

    m_countLabel->setText(tr("%1 devices").arg(m_proxy->rowCount()));
}

void ScanResultView::onSelectionChanged()
{
    auto indexes = m_treeView->selectionModel()->selectedRows();
    if (!indexes.isEmpty()) {
        auto device = indexes.first().data(Qt::UserRole).value<aips::core::Device>();
        emit selectionChanged(device);
    }
}

} // namespace aips::ui

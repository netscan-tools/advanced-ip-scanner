// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "ui/FavoritesPanel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

namespace aips::ui {

FavoritesPanel::FavoritesPanel(std::shared_ptr<aips::core::DeviceStore> store,
                                QWidget* parent)
    : QWidget(parent)
    , m_store(std::move(store))
{
    setupUi();
}

void FavoritesPanel::setupUi()
{
    auto* layout = new QVBoxLayout(this);

    auto* header = new QLabel(tr("Favorites"), this);
    header->setStyleSheet("font-weight: bold; font-size: 12px;");
    layout->addWidget(header);

    m_listWidget = new QListWidget(this);
    m_listWidget->setAlternatingRowColors(true);
    layout->addWidget(m_listWidget, 1);

    auto* btnLayout = new QHBoxLayout();

    m_removeBtn = new QPushButton(tr("Remove"), this);
    m_scanBtn   = new QPushButton(tr("Scan Favorites"), this);

    btnLayout->addWidget(m_removeBtn);
    btnLayout->addWidget(m_scanBtn);
    layout->addLayout(btnLayout);

    connect(m_listWidget, &QListWidget::itemDoubleClicked,
            this, &FavoritesPanel::onItemDoubleClicked);
    connect(m_removeBtn, &QPushButton::clicked,
            this, &FavoritesPanel::onRemoveClicked);
    connect(m_scanBtn, &QPushButton::clicked,
            this, &FavoritesPanel::onScanFavoritesClicked);
}

void FavoritesPanel::refresh()
{
    m_listWidget->clear();

    auto favorites = m_store->favoriteDevices();
    for (const auto& dev : favorites) {
        QString label = dev.hostname().isEmpty()
                        ? dev.ipAddress().toString()
                        : QString("%1 (%2)").arg(dev.hostname(), dev.ipAddress().toString());

        auto* item = new QListWidgetItem(label, m_listWidget);
        item->setData(Qt::UserRole, QVariant::fromValue(dev));
    }
}

void FavoritesPanel::addFavorite(const aips::core::Device& device)
{
    m_store->setFavorite(device.ipAddress(), true);
    refresh();
}

void FavoritesPanel::removeFavorite(const QHostAddress& address)
{
    m_store->setFavorite(address, false);
    refresh();
}

QVector<QHostAddress> FavoritesPanel::favoriteAddresses() const
{
    QVector<QHostAddress> addresses;
    auto favorites = m_store->favoriteDevices();
    for (const auto& dev : favorites)
        addresses.append(dev.ipAddress());
    return addresses;
}

bool FavoritesPanel::loadFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray arr = doc.array();

    for (const auto& val : arr) {
        QJsonObject obj = val.toObject();
        aips::core::Device dev = aips::core::Device::fromJson(obj);
        dev.setFavorite(true);
        m_store->addDevice(dev);
    }

    refresh();
    return true;
}

bool FavoritesPanel::saveToFile(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QJsonArray arr;
    auto favorites = m_store->favoriteDevices();
    for (const auto& dev : favorites)
        arr.append(dev.toJson());

    file.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    return true;
}

void FavoritesPanel::onItemDoubleClicked(QListWidgetItem* item)
{
    auto device = item->data(Qt::UserRole).value<aips::core::Device>();
    emit favoriteSelected(device);
}

void FavoritesPanel::onRemoveClicked()
{
    auto* current = m_listWidget->currentItem();
    if (!current)
        return;

    auto device = current->data(Qt::UserRole).value<aips::core::Device>();
    removeFavorite(device.ipAddress());
}

void FavoritesPanel::onScanFavoritesClicked()
{
    auto addresses = favoriteAddresses();
    if (!addresses.isEmpty())
        emit scanFavoritesRequested(addresses);
}

} // namespace aips::ui

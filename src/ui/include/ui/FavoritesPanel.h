// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/Device.h"
#include "core/DeviceStore.h"

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

#include <memory>

namespace aips::ui {

class FavoritesPanel : public QWidget {
    Q_OBJECT

public:
    explicit FavoritesPanel(std::shared_ptr<aips::core::DeviceStore> store,
                            QWidget* parent = nullptr);
    ~FavoritesPanel() override = default;

    void refresh();
    void addFavorite(const aips::core::Device& device);
    void removeFavorite(const QHostAddress& address);

    [[nodiscard]] QVector<QHostAddress> favoriteAddresses() const;

    bool loadFromFile(const QString& filePath);
    bool saveToFile(const QString& filePath) const;

signals:
    void favoriteSelected(const aips::core::Device& device);
    void scanFavoritesRequested(const QVector<QHostAddress>& addresses);

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onRemoveClicked();
    void onScanFavoritesClicked();

private:
    void setupUi();

    std::shared_ptr<aips::core::DeviceStore> m_store;
    QListWidget*  m_listWidget = nullptr;
    QPushButton*  m_removeBtn  = nullptr;
    QPushButton*  m_scanBtn    = nullptr;
};

} // namespace aips::ui

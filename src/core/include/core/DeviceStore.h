// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/Device.h"

#include <QObject>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QMutex>
#include <QJsonArray>

#include <optional>
#include <functional>

namespace aips::core {

class DeviceStore : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column : int {
        ColStatus = 0,
        ColName,
        ColIpAddress,
        ColMacAddress,
        ColVendor,
        ColNetbiosName,
        ColWorkgroup,
        ColOs,
        ColLatency,
        ColComment,
        ColLastSeen,
        ColumnCount
    };

    explicit DeviceStore(QObject* parent = nullptr);
    ~DeviceStore() override = default;

    [[nodiscard]] int rowCount(const QModelIndex& parent = {}) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    void addDevice(const Device& device);
    void updateDevice(const Device& device);
    void removeDevice(const QHostAddress& address);
    void clear();

    [[nodiscard]] std::optional<Device> findByIp(const QHostAddress& address) const;
    [[nodiscard]] std::optional<Device> findByMac(const MacAddress& mac) const;
    [[nodiscard]] QVector<Device> allDevices() const;
    [[nodiscard]] QVector<Device> favoriteDevices() const;
    [[nodiscard]] QVector<Device> onlineDevices() const;
    [[nodiscard]] int deviceCount() const;

    void setFavorite(const QHostAddress& address, bool favorite);
    void setComment(const QHostAddress& address, const QString& comment);

    [[nodiscard]] QJsonArray exportToJson() const;
    void importFromJson(const QJsonArray& arr);
    bool exportToCsv(const QString& filePath) const;
    bool exportToXml(const QString& filePath) const;
    bool importFromCsv(const QString& filePath);

    void mergeStore(const DeviceStore& other);

signals:
    void deviceAdded(const Device& device);
    void deviceChanged(const Device& device);
    void deviceRemoved(const QHostAddress& address);
    void storeCleared();

private:
    mutable QMutex      m_mutex;
    QVector<Device>     m_devices;
    QHash<quint32, int> m_ipIndex;

    int findIndex(const QHostAddress& address) const;
    void rebuildIndex();
};


class DeviceFilterProxy : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit DeviceFilterProxy(QObject* parent = nullptr);

    void setStatusFilter(DeviceStatus status);
    void setTypeFilter(DeviceType type);
    void setFavoritesOnly(bool enabled);
    void setSearchText(const QString& text);
    void clearFilters();

protected:
    [[nodiscard]] bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    [[nodiscard]] bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

private:
    std::optional<DeviceStatus> m_statusFilter;
    std::optional<DeviceType>   m_typeFilter;
    bool                        m_favoritesOnly = false;
    QString                     m_searchText;
};

} // namespace aips::core

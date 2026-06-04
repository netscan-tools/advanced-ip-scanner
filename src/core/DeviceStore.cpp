// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "core/DeviceStore.h"

#include <QFile>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QJsonDocument>

#include <algorithm>

namespace aips::core {

DeviceStore::DeviceStore(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int DeviceStore::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    QMutexLocker lock(&m_mutex);
    return m_devices.size();
}

int DeviceStore::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant DeviceStore::data(const QModelIndex& index, int role) const
{
    QMutexLocker lock(&m_mutex);

    if (!index.isValid() || index.row() >= m_devices.size())
        return {};

    const Device& dev = m_devices[index.row()];

    if (role == Qt::DisplayRole) {
        switch (static_cast<Column>(index.column())) {
            case ColStatus:      return dev.statusString();
            case ColName:        return dev.hostname().isEmpty() ? dev.ipAddress().toString() : dev.hostname();
            case ColIpAddress:   return dev.ipAddress().toString();
            case ColMacAddress:  return dev.macAddressString();
            case ColVendor:      return dev.vendor();
            case ColNetbiosName: return dev.netbiosName();
            case ColWorkgroup:   return dev.workgroup();
            case ColOs:          return dev.osFamily();
            case ColLatency:     return dev.latencyMs() > 0 ? QString::number(dev.latencyMs(), 'f', 1) + " ms" : QString();
            case ColComment:     return dev.comment();
            case ColLastSeen:    return dev.lastSeen().toString("yyyy-MM-dd hh:mm:ss");
            default: break;
        }
    }

    if (role == Qt::DecorationRole && index.column() == ColStatus) {
        switch (dev.status()) {
            case DeviceStatus::Online:  return QColor(Qt::green);
            case DeviceStatus::Offline: return QColor(Qt::red);
            default:                    return QColor(Qt::gray);
        }
    }

    if (role == Qt::UserRole)
        return QVariant::fromValue(dev);

    return {};
}

QVariant DeviceStore::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    switch (static_cast<Column>(section)) {
        case ColStatus:      return tr("Status");
        case ColName:        return tr("Name");
        case ColIpAddress:   return tr("IP Address");
        case ColMacAddress:  return tr("MAC Address");
        case ColVendor:      return tr("Manufacturer");
        case ColNetbiosName: return tr("NetBIOS Name");
        case ColWorkgroup:   return tr("Workgroup");
        case ColOs:          return tr("OS");
        case ColLatency:     return tr("Latency");
        case ColComment:     return tr("Comments");
        case ColLastSeen:    return tr("Last Seen");
        default: break;
    }
    return {};
}

Qt::ItemFlags DeviceStore::flags(const QModelIndex& index) const
{
    auto defaultFlags = QAbstractTableModel::flags(index);
    if (index.column() == ColComment)
        defaultFlags |= Qt::ItemIsEditable;
    return defaultFlags;
}

bool DeviceStore::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole || index.column() != ColComment)
        return false;

    QMutexLocker lock(&m_mutex);
    if (index.row() >= m_devices.size())
        return false;

    m_devices[index.row()].setComment(value.toString());
    emit dataChanged(index, index, {role});
    return true;
}

void DeviceStore::addDevice(const Device& device)
{
    QMutexLocker lock(&m_mutex);

    int row = m_devices.size();
    beginInsertRows({}, row, row);
    m_devices.append(device);
    m_ipIndex[device.ipAddress().toIPv4Address()] = row;
    endInsertRows();

    emit deviceAdded(device);
}

void DeviceStore::updateDevice(const Device& device)
{
    QMutexLocker lock(&m_mutex);

    int idx = findIndex(device.ipAddress());
    if (idx < 0)
        return;

    bool wasFavorite = m_devices[idx].isFavorite();
    m_devices[idx] = device;
    m_devices[idx].setFavorite(wasFavorite);

    QModelIndex topLeft     = index(idx, 0);
    QModelIndex bottomRight = index(idx, ColumnCount - 1);
    emit dataChanged(topLeft, bottomRight);
    emit deviceChanged(device);
}

void DeviceStore::removeDevice(const QHostAddress& address)
{
    QMutexLocker lock(&m_mutex);

    int idx = findIndex(address);
    if (idx < 0)
        return;

    beginRemoveRows({}, idx, idx);
    m_devices.removeAt(idx);
    rebuildIndex();
    endRemoveRows();

    emit deviceRemoved(address);
}

void DeviceStore::clear()
{
    QMutexLocker lock(&m_mutex);

    beginResetModel();
    m_devices.clear();
    m_ipIndex.clear();
    endResetModel();

    emit storeCleared();
}

std::optional<Device> DeviceStore::findByIp(const QHostAddress& address) const
{
    QMutexLocker lock(&m_mutex);
    int idx = findIndex(address);
    return idx >= 0 ? std::optional<Device>(m_devices[idx]) : std::nullopt;
}

std::optional<Device> DeviceStore::findByMac(const MacAddress& mac) const
{
    QMutexLocker lock(&m_mutex);
    auto it = std::find_if(m_devices.begin(), m_devices.end(),
                            [&mac](const Device& d) { return d.macAddress() == mac; });
    return it != m_devices.end() ? std::optional<Device>(*it) : std::nullopt;
}

QVector<Device> DeviceStore::allDevices() const
{
    QMutexLocker lock(&m_mutex);
    return m_devices;
}

QVector<Device> DeviceStore::favoriteDevices() const
{
    QMutexLocker lock(&m_mutex);
    QVector<Device> result;
    std::copy_if(m_devices.begin(), m_devices.end(), std::back_inserter(result),
                 [](const Device& d) { return d.isFavorite(); });
    return result;
}

QVector<Device> DeviceStore::onlineDevices() const
{
    QMutexLocker lock(&m_mutex);
    QVector<Device> result;
    std::copy_if(m_devices.begin(), m_devices.end(), std::back_inserter(result),
                 [](const Device& d) { return d.status() == DeviceStatus::Online; });
    return result;
}

int DeviceStore::deviceCount() const
{
    QMutexLocker lock(&m_mutex);
    return m_devices.size();
}

void DeviceStore::setFavorite(const QHostAddress& address, bool favorite)
{
    QMutexLocker lock(&m_mutex);
    int idx = findIndex(address);
    if (idx >= 0) {
        m_devices[idx].setFavorite(favorite);
        QModelIndex mi = index(idx, ColStatus);
        emit dataChanged(mi, mi);
    }
}

void DeviceStore::setComment(const QHostAddress& address, const QString& comment)
{
    QMutexLocker lock(&m_mutex);
    int idx = findIndex(address);
    if (idx >= 0) {
        m_devices[idx].setComment(comment);
        QModelIndex mi = index(idx, ColComment);
        emit dataChanged(mi, mi);
    }
}

QJsonArray DeviceStore::exportToJson() const
{
    QMutexLocker lock(&m_mutex);
    QJsonArray arr;
    for (const auto& dev : m_devices)
        arr.append(dev.toJson());
    return arr;
}

void DeviceStore::importFromJson(const QJsonArray& arr)
{
    for (const auto& val : arr) {
        Device dev = Device::fromJson(val.toObject());
        addDevice(dev);
    }
}

bool DeviceStore::exportToCsv(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << "Status,Name,IP Address,MAC Address,Manufacturer,NetBIOS,Workgroup,OS,Latency (ms),Comments,Last Seen\n";

    QMutexLocker lock(&m_mutex);
    for (const auto& dev : m_devices) {
        out << dev.statusString() << ","
            << "\"" << dev.hostname() << "\","
            << dev.ipAddress().toString() << ","
            << dev.macAddressString() << ","
            << "\"" << dev.vendor() << "\","
            << "\"" << dev.netbiosName() << "\","
            << "\"" << dev.workgroup() << "\","
            << "\"" << dev.osFamily() << "\","
            << dev.latencyMs() << ","
            << "\"" << dev.comment() << "\","
            << dev.lastSeen().toString(Qt::ISODate) << "\n";
    }

    return true;
}

bool DeviceStore::exportToXml(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("ScanResults");
    xml.writeAttribute("version", "2.5");

    QMutexLocker lock(&m_mutex);
    for (const auto& dev : m_devices) {
        xml.writeStartElement("Device");
        xml.writeTextElement("IP", dev.ipAddress().toString());
        xml.writeTextElement("MAC", dev.macAddressString());
        xml.writeTextElement("Hostname", dev.hostname());
        xml.writeTextElement("Vendor", dev.vendor());
        xml.writeTextElement("Status", dev.statusString());
        xml.writeTextElement("OS", dev.osFamily());
        xml.writeTextElement("NetBIOS", dev.netbiosName());
        xml.writeTextElement("Workgroup", dev.workgroup());
        xml.writeTextElement("Latency", QString::number(dev.latencyMs()));
        xml.writeTextElement("LastSeen", dev.lastSeen().toString(Qt::ISODate));
        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeEndDocument();
    return true;
}

bool DeviceStore::importFromCsv(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    in.readLine(); // skip header

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        if (fields.size() < 4)
            continue;

        Device dev(QHostAddress(fields[2].trimmed()));
        dev.setHostname(fields[1].remove('"').trimmed());
        addDevice(dev);
    }

    return true;
}

void DeviceStore::mergeStore(const DeviceStore& other)
{
    auto otherDevices = other.allDevices();
    for (const auto& dev : otherDevices) {
        auto existing = findByIp(dev.ipAddress());
        if (existing.has_value()) {
            updateDevice(dev);
        } else {
            addDevice(dev);
        }
    }
}

int DeviceStore::findIndex(const QHostAddress& address) const
{
    auto it = m_ipIndex.find(address.toIPv4Address());
    return it != m_ipIndex.end() ? it.value() : -1;
}

void DeviceStore::rebuildIndex()
{
    m_ipIndex.clear();
    for (int i = 0; i < m_devices.size(); ++i)
        m_ipIndex[m_devices[i].ipAddress().toIPv4Address()] = i;
}

// --- DeviceFilterProxy ---

DeviceFilterProxy::DeviceFilterProxy(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

void DeviceFilterProxy::setStatusFilter(DeviceStatus status)
{
    m_statusFilter = status;
    invalidateFilter();
}

void DeviceFilterProxy::setTypeFilter(DeviceType type)
{
    m_typeFilter = type;
    invalidateFilter();
}

void DeviceFilterProxy::setFavoritesOnly(bool enabled)
{
    m_favoritesOnly = enabled;
    invalidateFilter();
}

void DeviceFilterProxy::setSearchText(const QString& text)
{
    m_searchText = text;
    invalidateFilter();
}

void DeviceFilterProxy::clearFilters()
{
    m_statusFilter.reset();
    m_typeFilter.reset();
    m_favoritesOnly = false;
    m_searchText.clear();
    invalidateFilter();
}

bool DeviceFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
    auto device = idx.data(Qt::UserRole).value<Device>();

    if (m_statusFilter.has_value() && device.status() != m_statusFilter.value())
        return false;

    if (m_typeFilter.has_value() && device.deviceType() != m_typeFilter.value())
        return false;

    if (m_favoritesOnly && !device.isFavorite())
        return false;

    if (!m_searchText.isEmpty()) {
        bool matches = device.hostname().contains(m_searchText, Qt::CaseInsensitive)
                    || device.ipAddress().toString().contains(m_searchText)
                    || device.macAddressString().contains(m_searchText, Qt::CaseInsensitive)
                    || device.vendor().contains(m_searchText, Qt::CaseInsensitive)
                    || device.netbiosName().contains(m_searchText, Qt::CaseInsensitive);
        if (!matches)
            return false;
    }

    return true;
}

bool DeviceFilterProxy::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    if (left.column() == DeviceStore::ColIpAddress) {
        auto devL = left.data(Qt::UserRole).value<Device>();
        auto devR = right.data(Qt::UserRole).value<Device>();
        return devL.ipAddress().toIPv4Address() < devR.ipAddress().toIPv4Address();
    }

    if (left.column() == DeviceStore::ColLatency) {
        auto devL = left.data(Qt::UserRole).value<Device>();
        auto devR = right.data(Qt::UserRole).value<Device>();
        return devL.latencyMs() < devR.latencyMs();
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

} // namespace aips::core

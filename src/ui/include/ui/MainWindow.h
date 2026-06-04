// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/ScanEngine.h"
#include "core/DeviceStore.h"
#include "core/ScanRange.h"

#include <QMainWindow>
#include <QSystemTrayIcon>

#include <memory>

class QLineEdit;
class QPushButton;
class QProgressBar;
class QStatusBar;
class QLabel;
class QMenu;
class QAction;
class QToolBar;
class QSplitter;
class QComboBox;

namespace aips::ui {

class ScanResultView;
class FavoritesPanel;
class DeviceDetailsPanel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onScanClicked();
    void onStopClicked();
    void onPauseClicked();
    void onScanStarted();
    void onScanFinished(int devicesFound, qint64 elapsedMs);
    void onScanError(const QString& message);
    void onProgressChanged(int percent);
    void onDeviceDiscovered(const aips::core::Device& device);
    void onDeviceDoubleClicked(const aips::core::Device& device);
    void onDeviceContextMenu(const aips::core::Device& device, const QPoint& pos);

    void onImportAddressList();
    void onExportResults();
    void onExportCsv();
    void onExportXml();
    void onPreferences();
    void onAbout();

    void onFavoriteToggle(const aips::core::Device& device);
    void onWakeOnLan(const aips::core::Device& device);
    void onRemoteDesktop(const aips::core::Device& device);
    void onRemoteShutdown(const aips::core::Device& device);
    void onBrowseShares(const aips::core::Device& device);

    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupTrayIcon();
    void setupConnections();
    void loadSettings();
    void saveSettings();
    void updateWindowTitle();
    void updateScanControls(bool scanning);

    aips::core::ScanRange parseRangeInput() const;

    // UI components
    QLineEdit*          m_rangeInput     = nullptr;
    QComboBox*          m_presetCombo    = nullptr;
    QPushButton*        m_scanButton     = nullptr;
    QPushButton*        m_stopButton     = nullptr;
    QPushButton*        m_pauseButton    = nullptr;
    QProgressBar*       m_progressBar    = nullptr;
    QLabel*             m_statusLabel    = nullptr;
    QToolBar*           m_toolBar        = nullptr;
    QSplitter*          m_splitter       = nullptr;

    ScanResultView*     m_resultView     = nullptr;
    FavoritesPanel*     m_favoritesPanel = nullptr;
    DeviceDetailsPanel* m_detailsPanel   = nullptr;

    QSystemTrayIcon*    m_trayIcon       = nullptr;

    // Actions
    QAction* m_actScan       = nullptr;
    QAction* m_actStop       = nullptr;
    QAction* m_actPause      = nullptr;
    QAction* m_actImport     = nullptr;
    QAction* m_actExport     = nullptr;
    QAction* m_actPrefs      = nullptr;
    QAction* m_actAbout      = nullptr;
    QAction* m_actQuit       = nullptr;

    // Engine
    std::unique_ptr<aips::core::ScanEngine>  m_engine;
    std::shared_ptr<aips::core::DeviceStore> m_deviceStore;
};

} // namespace aips::ui

// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "ui/MainWindow.h"
#include "ui/ScanResultView.h"
#include "ui/FavoritesPanel.h"
#include "ui/DeviceDetailsPanel.h"
#include "ui/PreferencesDialog.h"
#include "remote/RdpLauncher.h"
#include "remote/WakeOnLan.h"
#include "remote/RemoteShutdown.h"

#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QSplitter>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QUrl>

namespace aips::ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_engine(std::make_unique<aips::core::ScanEngine>())
    , m_deviceStore(std::make_shared<aips::core::DeviceStore>())
{
    m_engine->setDeviceStore(m_deviceStore);

    setupUi();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupTrayIcon();
    setupConnections();
    loadSettings();
    updateWindowTitle();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        if (m_engine->state() != aips::core::ScanEngine::ScanState::Idle) {
            auto reply = QMessageBox::question(
                this, tr("Scan in Progress"),
                tr("A scan is currently running. Do you want to stop and exit?"),
                QMessageBox::Yes | QMessageBox::No
            );
            if (reply == QMessageBox::No) {
                event->ignore();
                return;
            }
            m_engine->cancelScan();
        }
        saveSettings();
        event->accept();
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized() && m_trayIcon && m_trayIcon->isVisible())
            hide();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("Advanced IP Scanner"));
    setMinimumSize(900, 600);
    resize(1200, 750);

    auto* centralWidget = new QWidget(this);
    auto* mainLayout    = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(4, 4, 4, 4);

    // Scan bar
    auto* scanBar = new QHBoxLayout();

    m_rangeInput = new QLineEdit(this);
    m_rangeInput->setPlaceholderText(tr("Enter IP address, range (e.g. 192.168.1.1-254), or CIDR..."));
    m_rangeInput->setMinimumWidth(350);

    m_presetCombo = new QComboBox(this);
    m_presetCombo->addItem(tr("Custom Range"), -1);
    m_presetCombo->addItem(tr("192.168.1.0/24"), 0);
    m_presetCombo->addItem(tr("192.168.0.0/24"), 1);
    m_presetCombo->addItem(tr("10.0.0.0/24"), 2);
    m_presetCombo->addItem(tr("172.16.0.0/24"), 3);

    m_scanButton  = new QPushButton(tr("Scan"), this);
    m_stopButton  = new QPushButton(tr("Stop"), this);
    m_pauseButton = new QPushButton(tr("Pause"), this);

    m_scanButton->setMinimumWidth(80);
    m_stopButton->setMinimumWidth(80);
    m_pauseButton->setMinimumWidth(80);

    m_stopButton->setEnabled(false);
    m_pauseButton->setEnabled(false);

    scanBar->addWidget(m_rangeInput, 1);
    scanBar->addWidget(m_presetCombo);
    scanBar->addWidget(m_scanButton);
    scanBar->addWidget(m_pauseButton);
    scanBar->addWidget(m_stopButton);

    mainLayout->addLayout(scanBar);

    // Progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setFixedHeight(18);
    mainLayout->addWidget(m_progressBar);

    // Content area
    m_splitter = new QSplitter(Qt::Horizontal, this);

    m_favoritesPanel = new FavoritesPanel(m_deviceStore, this);
    m_resultView     = new ScanResultView(m_deviceStore, this);
    m_detailsPanel   = new DeviceDetailsPanel(this);

    m_favoritesPanel->setMaximumWidth(250);
    m_detailsPanel->setMaximumWidth(350);

    m_splitter->addWidget(m_favoritesPanel);
    m_splitter->addWidget(m_resultView);
    m_splitter->addWidget(m_detailsPanel);
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setStretchFactor(2, 0);

    mainLayout->addWidget(m_splitter, 1);

    setCentralWidget(centralWidget);
}

void MainWindow::setupMenuBar()
{
    auto* fileMenu = menuBar()->addMenu(tr("&File"));

    m_actImport = fileMenu->addAction(tr("&Import Address List..."), this, &MainWindow::onImportAddressList);
    m_actImport->setShortcut(QKeySequence("Ctrl+I"));

    m_actExport = fileMenu->addAction(tr("&Export Results..."), this, &MainWindow::onExportResults);
    m_actExport->setShortcut(QKeySequence("Ctrl+E"));

    fileMenu->addSeparator();

    m_actQuit = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    m_actQuit->setShortcut(QKeySequence::Quit);

    auto* scanMenu = menuBar()->addMenu(tr("&Scan"));

    m_actScan = scanMenu->addAction(tr("&Start Scan"), this, &MainWindow::onScanClicked);
    m_actScan->setShortcut(QKeySequence("F5"));

    m_actPause = scanMenu->addAction(tr("&Pause"), this, &MainWindow::onPauseClicked);
    m_actStop  = scanMenu->addAction(tr("S&top"), this, &MainWindow::onStopClicked);
    m_actStop->setShortcut(QKeySequence("Escape"));

    auto* toolsMenu = menuBar()->addMenu(tr("&Tools"));

    m_actPrefs = toolsMenu->addAction(tr("&Preferences..."), this, &MainWindow::onPreferences);
    m_actPrefs->setShortcut(QKeySequence::Preferences);

    auto* helpMenu = menuBar()->addMenu(tr("&Help"));

    m_actAbout = helpMenu->addAction(tr("&About Advanced IP Scanner"), this, &MainWindow::onAbout);
}

void MainWindow::setupToolBar()
{
    m_toolBar = addToolBar(tr("Main"));
    m_toolBar->setMovable(false);
    m_toolBar->setIconSize(QSize(24, 24));

    m_toolBar->addAction(m_actScan);
    m_toolBar->addAction(m_actPause);
    m_toolBar->addAction(m_actStop);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actImport);
    m_toolBar->addAction(m_actExport);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actPrefs);
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel(tr("Ready"), this);
    statusBar()->addPermanentWidget(m_statusLabel);
}

void MainWindow::setupTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setToolTip(tr("Advanced IP Scanner"));

    auto* trayMenu = new QMenu(this);
    trayMenu->addAction(tr("Show"), this, &QWidget::showNormal);
    trayMenu->addSeparator();
    trayMenu->addAction(tr("Exit"), qApp, &QCoreApplication::quit);
    m_trayIcon->setContextMenu(trayMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &MainWindow::onTrayActivated);
}

void MainWindow::setupConnections()
{
    connect(m_scanButton,  &QPushButton::clicked, this, &MainWindow::onScanClicked);
    connect(m_stopButton,  &QPushButton::clicked, this, &MainWindow::onStopClicked);
    connect(m_pauseButton, &QPushButton::clicked, this, &MainWindow::onPauseClicked);

    connect(m_engine.get(), &aips::core::ScanEngine::scanStarted,
            this, &MainWindow::onScanStarted);
    connect(m_engine.get(), &aips::core::ScanEngine::scanFinished,
            this, &MainWindow::onScanFinished);
    connect(m_engine.get(), &aips::core::ScanEngine::scanError,
            this, &MainWindow::onScanError);
    connect(m_engine.get(), &aips::core::ScanEngine::progressChanged,
            this, &MainWindow::onProgressChanged);
    connect(m_engine.get(), &aips::core::ScanEngine::deviceDiscovered,
            this, &MainWindow::onDeviceDiscovered);

    connect(m_resultView, &ScanResultView::deviceDoubleClicked,
            this, &MainWindow::onDeviceDoubleClicked);
    connect(m_resultView, &ScanResultView::deviceContextMenuRequested,
            this, &MainWindow::onDeviceContextMenu);
    connect(m_resultView, &ScanResultView::selectionChanged,
            m_detailsPanel, &DeviceDetailsPanel::showDevice);

    connect(m_detailsPanel, &DeviceDetailsPanel::rdpRequested,
            this, &MainWindow::onRemoteDesktop);
    connect(m_detailsPanel, &DeviceDetailsPanel::wolRequested,
            this, &MainWindow::onWakeOnLan);
    connect(m_detailsPanel, &DeviceDetailsPanel::shutdownRequested,
            this, &MainWindow::onRemoteShutdown);
    connect(m_detailsPanel, &DeviceDetailsPanel::browseSharesRequested,
            this, &MainWindow::onBrowseShares);

    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (idx > 0)
            m_rangeInput->setText(m_presetCombo->currentText());
    });
}

void MainWindow::loadSettings()
{
    QSettings s("Famatech", "AdvancedIPScanner");
    restoreGeometry(s.value("geometry").toByteArray());
    restoreState(s.value("windowState").toByteArray());
    m_rangeInput->setText(s.value("lastRange", "192.168.1.1-254").toString());

    if (s.value("splitterState").isValid())
        m_splitter->restoreState(s.value("splitterState").toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings s("Famatech", "AdvancedIPScanner");
    s.setValue("geometry", saveGeometry());
    s.setValue("windowState", saveState());
    s.setValue("lastRange", m_rangeInput->text());
    s.setValue("splitterState", m_splitter->saveState());
}

void MainWindow::onScanClicked()
{
    auto range = parseRangeInput();
    if (!range.isValid()) {
        QMessageBox::warning(this, tr("Invalid Range"),
                              tr("Please enter a valid IP address or range."));
        return;
    }

    m_engine->setScanRange(range);
    m_engine->startScan();
}

void MainWindow::onStopClicked()
{
    m_engine->cancelScan();
}

void MainWindow::onPauseClicked()
{
    if (m_engine->state() == aips::core::ScanEngine::ScanState::Paused) {
        m_engine->resumeScan();
        m_pauseButton->setText(tr("Pause"));
    } else {
        m_engine->pauseScan();
        m_pauseButton->setText(tr("Resume"));
    }
}

void MainWindow::onScanStarted()
{
    updateScanControls(true);
    m_statusLabel->setText(tr("Scanning..."));
    m_progressBar->setValue(0);
}

void MainWindow::onScanFinished(int devicesFound, qint64 elapsedMs)
{
    updateScanControls(false);
    double sec = elapsedMs / 1000.0;
    m_statusLabel->setText(
        tr("Scan complete: %1 devices found in %2 seconds")
            .arg(devicesFound)
            .arg(sec, 0, 'f', 1)
    );
    m_progressBar->setValue(100);
    m_favoritesPanel->refresh();
}

void MainWindow::onScanError(const QString& message)
{
    m_statusLabel->setText(tr("Error: %1").arg(message));
}

void MainWindow::onProgressChanged(int percent)
{
    m_progressBar->setValue(percent);
    m_statusLabel->setText(tr("Scanning... %1% (%2 devices found)")
                               .arg(percent)
                               .arg(m_engine->devicesFound()));
}

void MainWindow::onDeviceDiscovered(const aips::core::Device& device)
{
    Q_UNUSED(device);
    updateWindowTitle();
}

void MainWindow::onDeviceDoubleClicked(const aips::core::Device& device)
{
    if (!device.sharedResources().isEmpty()) {
        onBrowseShares(device);
    } else {
        m_detailsPanel->showDevice(device);
    }
}

void MainWindow::onDeviceContextMenu(const aips::core::Device& device, const QPoint& pos)
{
    QMenu menu(this);

    menu.addAction(tr("Remote Desktop (RDP)"), this, [this, device]() {
        onRemoteDesktop(device);
    });
    menu.addAction(tr("Wake-on-LAN"), this, [this, device]() {
        onWakeOnLan(device);
    });
    menu.addAction(tr("Browse Shares"), this, [this, device]() {
        onBrowseShares(device);
    });

    menu.addSeparator();

    menu.addAction(tr("Shutdown"), this, [this, device]() {
        onRemoteShutdown(device);
    });

    menu.addSeparator();

    auto favoriteText = device.isFavorite() ? tr("Remove from Favorites") : tr("Add to Favorites");
    menu.addAction(favoriteText, this, [this, device]() {
        onFavoriteToggle(device);
    });

    menu.addSeparator();

    menu.addAction(tr("Copy IP Address"), this, [&device]() {
        QApplication::clipboard()->setText(device.ipAddress().toString());
    });
    menu.addAction(tr("Copy MAC Address"), this, [&device]() {
        QApplication::clipboard()->setText(device.macAddressString());
    });

    menu.exec(pos);
}

void MainWindow::onImportAddressList()
{
    QString file = QFileDialog::getOpenFileName(
        this, tr("Import Address List"),
        QString(), tr("Text Files (*.txt);;All Files (*)")
    );

    if (!file.isEmpty()) {
        auto range = aips::core::ScanRange::fromFile(file);
        if (range.isValid()) {
            m_rangeInput->setText(file);
            m_engine->setScanRange(range);
            m_statusLabel->setText(tr("Loaded %1 addresses from file").arg(range.totalAddresses()));
        }
    }
}

void MainWindow::onExportResults()
{
    onExportCsv();
}

void MainWindow::onExportCsv()
{
    QString file = QFileDialog::getSaveFileName(
        this, tr("Export to CSV"),
        "scan_results.csv", tr("CSV Files (*.csv)")
    );

    if (!file.isEmpty()) {
        if (m_deviceStore->exportToCsv(file))
            m_statusLabel->setText(tr("Results exported to %1").arg(file));
        else
            QMessageBox::warning(this, tr("Export Failed"), tr("Could not write to file."));
    }
}

void MainWindow::onExportXml()
{
    QString file = QFileDialog::getSaveFileName(
        this, tr("Export to XML"),
        "scan_results.xml", tr("XML Files (*.xml)")
    );

    if (!file.isEmpty()) {
        if (m_deviceStore->exportToXml(file))
            m_statusLabel->setText(tr("Results exported to %1").arg(file));
    }
}

void MainWindow::onPreferences()
{
    aips::core::ScanOptions currentOpt;
    PreferencesDialog dlg(currentOpt, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_engine->setScanOptions(dlg.options());
        m_statusLabel->setText(tr("Preferences updated"));
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("About Advanced IP Scanner"),
        tr("<h2>Advanced IP Scanner %1</h2>"
           "<p>Free, fast and reliable network scanner.</p>"
           "<p>Copyright &copy; 2024 Famatech Corp.</p>"
           "<p><a href=\"https://www.advanced-ip-scanner.com\">www.advanced-ip-scanner.com</a></p>")
        .arg(QStringLiteral(AIPS_VERSION))
    );
}

void MainWindow::onFavoriteToggle(const aips::core::Device& device)
{
    m_deviceStore->setFavorite(device.ipAddress(), !device.isFavorite());
    m_favoritesPanel->refresh();
}

void MainWindow::onWakeOnLan(const aips::core::Device& device)
{
    remote::WakeOnLan wol;
    bool ok = wol.wake(device);
    m_statusLabel->setText(ok
        ? tr("Wake-on-LAN packet sent to %1").arg(device.macAddressString())
        : tr("Failed to send Wake-on-LAN packet"));
}

void MainWindow::onRemoteDesktop(const aips::core::Device& device)
{
    auto* rdp = new remote::RdpLauncher(this);
    rdp->launch(device.ipAddress());
}

void MainWindow::onRemoteShutdown(const aips::core::Device& device)
{
    auto reply = QMessageBox::question(
        this, tr("Remote Shutdown"),
        tr("Are you sure you want to shut down %1 (%2)?")
            .arg(device.hostname().isEmpty() ? device.ipAddress().toString() : device.hostname())
            .arg(device.ipAddress().toString()),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        remote::RemoteShutdown sd;
        sd.execute(device.ipAddress(), remote::RemoteShutdown::Action::Shutdown);
    }
}

void MainWindow::onBrowseShares(const aips::core::Device& device)
{
    if (!device.sharedResources().isEmpty()) {
        const auto& first = device.sharedResources().first();
        QDesktopServices::openUrl(QUrl(first.path));
    } else {
        QString path = QString("\\\\%1").arg(device.ipAddress().toString());
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        showNormal();
        activateWindow();
    }
}

void MainWindow::updateWindowTitle()
{
    int count = m_deviceStore->deviceCount();
    if (count > 0) {
        setWindowTitle(tr("Advanced IP Scanner — %1 devices").arg(count));
    } else {
        setWindowTitle(tr("Advanced IP Scanner"));
    }
}

void MainWindow::updateScanControls(bool scanning)
{
    m_scanButton->setEnabled(!scanning);
    m_stopButton->setEnabled(scanning);
    m_pauseButton->setEnabled(scanning);
    m_rangeInput->setEnabled(!scanning);
    m_presetCombo->setEnabled(!scanning);
    m_actScan->setEnabled(!scanning);
    m_actStop->setEnabled(scanning);
    m_actPause->setEnabled(scanning);
}

aips::core::ScanRange MainWindow::parseRangeInput() const
{
    QString text = m_rangeInput->text().trimmed();
    if (text.isEmpty())
        return {};

    if (QFile::exists(text))
        return aips::core::ScanRange::fromFile(text);

    auto parsed = aips::core::ScanRange::parse(text);
    return parsed.value_or(aips::core::ScanRange());
}

} // namespace aips::ui

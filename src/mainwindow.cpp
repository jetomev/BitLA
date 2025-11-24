#include "mainwindow.h"
#include "minerconfigdialog.h"

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QPalette>
#include <QPlainTextEdit>
#include <QRandomGenerator>
#include <QSplitter>
#include <QStatusBar>
#include <QKeySequence>
#include <QSize>
#include <QTextCursor>
#include <QTextStream>
#include <QVBoxLayout>

namespace {
QString formatDuration(int seconds)
{
    const int hours = seconds / 3600;
    const int minutes = (seconds % 3600) / 60;
    const int remainingSeconds = seconds % 60;
    return QString("%1h %2m %3s").arg(hours).arg(minutes, 2, 10, QLatin1Char('0')).arg(remainingSeconds, 2, 10, QLatin1Char('0'));
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("The Bitcoin Lottery Application (BitLA)"));
    resize(1200, 800);

    createMenus();
    createActionBar();

    auto *splitter = new QSplitter;
    splitter->addWidget(createDashboardPane());
    splitter->addWidget(createLogPane());
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    setCentralWidget(splitter);

    connect(&simulationTimer, &QTimer::timeout, this, &MainWindow::appendSimulatedData);
    simulationTimer.setInterval(1000);

    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createMenus()
{
    auto *fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(tr("Save Log As"), this, &MainWindow::saveLogAs, QKeySequence::Save);
    fileMenu->addAction(tr("Delete Log"), this, &MainWindow::clearLog);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Exit"), this, &QWidget::close, QKeySequence::Quit);

    auto *viewMenu = menuBar()->addMenu(tr("View"));
    viewMenu->addAction(tr("Dashboard"), [this]() { statusBar()->showMessage(tr("Dashboard is visible."), 2000); });
    viewMenu->addAction(tr("Log"), [this]() { statusBar()->showMessage(tr("Log is visible."), 2000); });
    viewMenu->addAction(tr("About Minerd"), this, &MainWindow::showAboutMinerd);

    auto *setupMenu = menuBar()->addMenu(tr("Setup"));
    setupMenu->addAction(tr("Install Minerd"), this, &MainWindow::showInstallMinerd);
    setupMenu->addAction(tr("Configure Miner"), this, &MainWindow::openConfigureMiner);
    switchThemeAction = setupMenu->addAction(tr("Switch Theme"), this, &MainWindow::switchTheme);

    auto *aboutMenu = menuBar()->addMenu(tr("About"));
    aboutMenu->addAction(tr("About BitLA"), this, &MainWindow::showAboutApp);
}

void MainWindow::createActionBar()
{
    auto *toolbar = new QToolBar(tr("Actions"));
    toolbar->setOrientation(Qt::Vertical);
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(32, 32));
    addToolBar(Qt::LeftToolBarArea, toolbar);

    startAction = toolbar->addAction(tr("Start Miner"), this, &MainWindow::startMiner);
    stopAction = toolbar->addAction(tr("Stop Miner"), this, &MainWindow::stopMiner);
    stopAction->setEnabled(false);
}

QWidget *MainWindow::createDashboardPane()
{
    auto *container = new QWidget;
    auto *outerLayout = new QVBoxLayout(container);

    auto *dashboardBox = new QGroupBox(tr("Dashboard"));
    auto *grid = new QGridLayout(dashboardBox);

    auto makeRow = [grid](int row, const QString &labelText, QLabel **valueLabel) {
        auto *label = new QLabel(labelText + ":");
        *valueLabel = new QLabel(QObject::tr("--"));
        grid->addWidget(label, row, 0);
        grid->addWidget(*valueLabel, row, 1);
    };

    int row = 0;
    makeRow(row++, tr("Pool Name"), &poolNameValue);
    makeRow(row++, tr("Pool Port"), &poolPortValue);
    makeRow(row++, tr("Wallet"), &walletValue);
    makeRow(row++, tr("CPU Load"), &cpuLoadValue);
    makeRow(row++, tr("Threads"), &threadsValue);
    makeRow(row++, tr("Uptime"), &uptimeValue);
    makeRow(row++, tr("Hashrate"), &hashrateValue);
    makeRow(row++, tr("Shares (A/R)"), &sharesValue);
    makeRow(row++, tr("Log Lines"), &logLinesValue);

    outerLayout->addWidget(dashboardBox);

    outerLayout->addWidget(createGraphPane());

    outerLayout->addStretch();
    return container;
}

QWidget *MainWindow::createGraphPane()
{
    auto *graphBox = new QGroupBox(tr("Graph & Stats"));
    auto *layout = new QVBoxLayout(graphBox);

    graphStatusLabel = new QLabel(tr("Live graph coming soon."));
    graphStatusLabel->setAlignment(Qt::AlignCenter);

    refreshCombo = new QComboBox;
    refreshCombo->addItems({tr("1s"), tr("5s"), tr("10s"), tr("30s"), tr("1h")});
    refreshCombo->setCurrentIndex(1);
    connect(refreshCombo, &QComboBox::currentTextChanged, this, [this](const QString &value) {
        if (value.endsWith('s')) {
            const QString seconds = value.left(value.size() - 1);
            simulationTimer.setInterval(seconds.toInt() * 1000);
        } else if (value.endsWith('h')) {
            const QString hours = value.left(value.size() - 1);
            simulationTimer.setInterval(hours.toInt() * 3600 * 1000);
        }
    });

    miniStatsLabel = new QLabel(tr("Peak/Avg stats will appear here."));
    miniStatsLabel->setWordWrap(true);

    layout->addWidget(graphStatusLabel);
    layout->addWidget(miniStatsLabel);

    auto *refreshLayout = new QHBoxLayout;
    refreshLayout->addWidget(new QLabel(tr("Refresh:")));
    refreshLayout->addWidget(refreshCombo);
    refreshLayout->addStretch();
    layout->addLayout(refreshLayout);

    return graphBox;
}

QWidget *MainWindow::createLogPane()
{
    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);

    auto *searchLayout = new QHBoxLayout;
    searchLayout->addWidget(new QLabel(tr("Search/Filter:")));
    searchField = new QLineEdit;
    searchField->setPlaceholderText(tr("Type to filter log lines"));
    connect(searchField, &QLineEdit::textChanged, this, &MainWindow::applySearchFilter);
    searchLayout->addWidget(searchField);

    autoScrollCheck = new QCheckBox(tr("Auto-scroll"));
    autoScrollCheck->setChecked(true);
    searchLayout->addWidget(autoScrollCheck);

    layout->addLayout(searchLayout);

    logView = new QPlainTextEdit;
    logView->setReadOnly(true);
    logView->setMaximumBlockCount(kMaxLogLines);

    layout->addWidget(logView, 1);
    return container;
}

void MainWindow::startMiner()
{
    if (minerRunning) {
        return;
    }

    minerRunning = true;
    simulatedUptimeSeconds = 0;
    simulatedHashrate = 0.0;
    simulatedAccepted = 0;
    simulatedRejected = 0;

    startAction->setEnabled(false);
    stopAction->setEnabled(true);
    statusBar()->showMessage(tr("Miner started (simulation)"), 2000);

    appendLogLine(tr("Miner starting with selected configuration."));
    simulationTimer.start();
}

void MainWindow::stopMiner()
{
    if (!minerRunning) {
        return;
    }

    minerRunning = false;
    simulationTimer.stop();

    startAction->setEnabled(true);
    stopAction->setEnabled(false);
    statusBar()->showMessage(tr("Miner stopped"), 2000);
    appendLogLine(tr("Miner stopped."));
}

void MainWindow::saveLogAs()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save Log"), QString(), tr("Text Files (*.txt);;All Files (*)"));
    if (path.isEmpty()) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Unable to save"), tr("Could not open %1 for writing").arg(path));
        return;
    }

    QTextStream stream(&file);
    for (const auto &line : logBuffer) {
        stream << line << '\n';
    }
}

void MainWindow::clearLog()
{
    logBuffer.clear();
    updateLogDisplay();
    appendLogLine(tr("Log cleared."));
}

void MainWindow::showAboutApp()
{
    QMessageBox::about(this, tr("About BitLA"), tr("<b>The Bitcoin Lottery Application</b><br/>KDE/Qt dashboard for running and monitoring minerd."));
}

void MainWindow::showAboutMinerd()
{
    QMessageBox::information(this, tr("About Minerd"), tr("Minerd command flags will be summarized here. Configure Miner shows currently selected options."));
}

void MainWindow::showInstallMinerd()
{
    QMessageBox::information(this, tr("Install Minerd"), tr("Coming soon: guided installer for minerd on Arch Linux/AUR."));
}

void MainWindow::openConfigureMiner()
{
    MinerConfigDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        const auto summary = tr("Pool: %1\nWallet: %2\nAlgo: %3\nThreads: %4").arg(dialog.poolUrl(), dialog.walletAddress(), dialog.algo(), dialog.threads());
        appendLogLine(tr("Configuration updated."));
        statusBar()->showMessage(summary, 5000);
    }
}

void MainWindow::switchTheme()
{
    const QStringList options{tr("Light"), tr("Dark"), tr("System")};
    bool ok = false;
    const QString choice = QInputDialog::getItem(this, tr("Switch Theme"), tr("Theme"), options, 0, false, &ok);
    if (!ok || choice.isEmpty()) {
        return;
    }

    setThemeFromSelection(choice);
}

void MainWindow::applySearchFilter(const QString &)
{
    updateLogDisplay();
}

void MainWindow::appendSimulatedData()
{
    if (!minerRunning) {
        return;
    }

    auto *rng = QRandomGenerator::global();
    simulatedUptimeSeconds += simulationTimer.interval() / 1000;
    simulatedHashrate = 50.0 + rng->bounded(2000) / 10.0;
    if (rng->bounded(4) == 0) {
        ++simulatedAccepted;
    } else if (rng->bounded(20) == 0) {
        ++simulatedRejected;
    }

    appendLogLine(tr("[%1] Hashrate: %2 kH/s | Shares A/R: %3/%4")
                      .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                      .arg(simulatedHashrate, 0, 'f', 1)
                      .arg(simulatedAccepted)
                      .arg(simulatedRejected));

    updateDashboard();
}

void MainWindow::appendLogLine(const QString &line)
{
    logBuffer.append(line);
    while (logBuffer.size() > kMaxLogLines) {
        logBuffer.removeFirst();
    }

    updateLogDisplay();
}

void MainWindow::updateLogDisplay()
{
    if (!logView) {
        return;
    }

    const QString filter = searchField ? searchField->text() : QString();

    QStringList filtered;
    if (filter.isEmpty()) {
        filtered = logBuffer;
    } else {
        for (const auto &line : logBuffer) {
            if (line.contains(filter, Qt::CaseInsensitive)) {
                filtered.append(line);
            }
        }
    }

    logView->setPlainText(filtered.join('\n'));

    if (autoScrollCheck && autoScrollCheck->isChecked()) {
        logView->moveCursor(QTextCursor::End);
    }

    updateDashboard();
}

void MainWindow::updateDashboard()
{
    if (poolNameValue) {
        poolNameValue->setText(tr("Configured pool"));
    }
    if (poolPortValue) {
        poolPortValue->setText(tr("3333"));
    }
    if (walletValue) {
        walletValue->setText(tr("Wallet configured"));
    }
    if (cpuLoadValue) {
        cpuLoadValue->setText(tr("%1 %").arg(20 + (QRandomGenerator::global()->bounded(60))));
    }
    if (threadsValue) {
        threadsValue->setText(tr("%1").arg(4));
    }
    if (uptimeValue) {
        uptimeValue->setText(minerRunning ? formatDuration(simulatedUptimeSeconds) : tr("--"));
    }
    if (hashrateValue) {
        hashrateValue->setText(minerRunning ? tr("%1 kH/s").arg(simulatedHashrate, 0, 'f', 1) : tr("--"));
    }
    if (sharesValue) {
        sharesValue->setText(tr("%1 / %2").arg(simulatedAccepted).arg(simulatedRejected));
    }
    if (logLinesValue) {
        logLinesValue->setText(tr("%1").arg(logBuffer.size()));
    }
    if (miniStatsLabel) {
        miniStatsLabel->setText(tr("Refresh: %1 | Peak Hashrate: %2 kH/s (simulated)")
                                    .arg(refreshCombo ? refreshCombo->currentText() : tr("--"))
                                    .arg(simulatedHashrate, 0, 'f', 1));
    }
}

void MainWindow::setLightTheme()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(245, 247, 250));
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(230, 234, 239));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(230, 234, 239));
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::Highlight, QColor(61, 131, 246));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    qApp->setPalette(palette);
}

void MainWindow::setDarkTheme()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(45, 45, 48));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(30, 30, 30));
    palette.setColor(QPalette::AlternateBase, QColor(45, 45, 48));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(60, 60, 60));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    qApp->setPalette(palette);
}

void MainWindow::setSystemTheme()
{
    qApp->setPalette(QPalette());
}

void MainWindow::setThemeFromSelection(const QString &themeName)
{
    if (themeName.compare(tr("Light"), Qt::CaseInsensitive) == 0) {
        setLightTheme();
    } else if (themeName.compare(tr("Dark"), Qt::CaseInsensitive) == 0) {
        setDarkTheme();
    } else {
        setSystemTheme();
    }
}
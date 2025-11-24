#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPointer>
#include <QPushButton>
#include <QSplitter>
#include <QStringList>
#include <QTimer>
#include <QToolBar>

class MinerConfigDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void startMiner();
    void stopMiner();
    void saveLogAs();
    void clearLog();
    void showAboutApp();
    void showAboutMinerd();
    void showInstallMinerd();
    void openConfigureMiner();
    void switchTheme();
    void applySearchFilter(const QString &text);
    void appendSimulatedData();

private:
    void createMenus();
    void createActionBar();
    QWidget *createDashboardPane();
    QWidget *createGraphPane();
    QWidget *createLogPane();
    void appendLogLine(const QString &line);
    void updateLogDisplay();
    void updateDashboard();
    void setLightTheme();
    void setDarkTheme();
    void setSystemTheme();
    void setThemeFromSelection(const QString &themeName);

    QStringList logBuffer;
    static constexpr int kMaxLogLines = 5000;

    QPointer<QPlainTextEdit> logView;
    QPointer<QLineEdit> searchField;
    QPointer<QCheckBox> autoScrollCheck;
    QPointer<QComboBox> refreshCombo;

    QPointer<QLabel> poolNameValue;
    QPointer<QLabel> poolPortValue;
    QPointer<QLabel> walletValue;
    QPointer<QLabel> cpuLoadValue;
    QPointer<QLabel> threadsValue;
    QPointer<QLabel> uptimeValue;
    QPointer<QLabel> hashrateValue;
    QPointer<QLabel> sharesValue;
    QPointer<QLabel> logLinesValue;

    QPointer<QLabel> graphStatusLabel;
    QPointer<QLabel> miniStatsLabel;

    QPointer<QAction> startAction;
    QPointer<QAction> stopAction;
    QPointer<QAction> switchThemeAction;

    QTimer simulationTimer;
    bool minerRunning = false;
    int simulatedUptimeSeconds = 0;
    double simulatedHashrate = 0.0;
    int simulatedAccepted = 0;
    int simulatedRejected = 0;
};

#endif // MAINWINDOW_H

#include "ecg_app.h"
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QTabWidget>

ecg_app::ecg_app(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_ecg_app)
    , m_mqttHost("localhost")
    , m_mqttPort(1883)
    , m_cloudServerUrl("https://ecg-cloud.com")
{
    ui->setupUi(this);
    
    initializeModules();
    setupUI();
    connectSignals();
    loadSettings();
}

ecg_app::~ecg_app()
{
    saveSettings();
    delete ui; 
}

void ecg_app::initializeModules() {
#ifndef NO_MQTT_SUPPORT
    // 初始化MQTT客户端
    m_mqttClient = new MqttClientManager(this);
#endif
    
    // 初始化数据库
    m_database = new DatabaseManager(this);
    m_database->initialize();
    
    // 初始化云同步
    m_cloudSync = new CloudSyncManager(this);
    m_cloudSync->setServerUrl(m_cloudServerUrl);
    m_cloudSync->enableAutoSync(true, 5); // 每5分钟自动同步
    
    // 初始化UI组件
    m_realtimeChart = new ChartWidget(this);
    m_ecgWaveform = new ECGWaveformWidget(this);
    m_vitalSignPanel = new VitalSignPanel(this);
    m_historyChart = new ChartWidget(this);
}

void ecg_app::setupUI() {
    // 实时监护标签页
    QWidget* realtimeTab = ui->tab_realtime;
    QVBoxLayout* realtimeLayout = new QVBoxLayout(realtimeTab);
    
    QSplitter* splitter = new QSplitter(Qt::Horizontal, realtimeTab);
    
    // 左侧：数值显示
    splitter->addWidget(m_vitalSignPanel);
    
    // 右侧：图表显示
    QWidget* chartContainer = new QWidget();
    QVBoxLayout* chartLayout = new QVBoxLayout(chartContainer);
    chartLayout->addWidget(m_ecgWaveform, 2);
    chartLayout->addWidget(m_realtimeChart, 1);
    splitter->addWidget(chartContainer);
    
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    
    realtimeLayout->addWidget(splitter);
    
    // 历史查询标签页
    QWidget* historyTab = ui->tab_history;
    QVBoxLayout* historyLayout = new QVBoxLayout(historyTab);
    
    // 添加时间选择和查询按钮
    QHBoxLayout* queryLayout = new QHBoxLayout();
    QPushButton* btnQuery = new QPushButton("查询", historyTab);
    QPushButton* btnExport = new QPushButton("导出CSV", historyTab);
    queryLayout->addStretch();
    queryLayout->addWidget(btnQuery);
    queryLayout->addWidget(btnExport);
    
    historyLayout->addLayout(queryLayout);
    historyLayout->addWidget(m_historyChart);
    
    connect(btnQuery, &QPushButton::clicked, this, [this]() {
        // 查询最近24小时的数据
        QDateTime endTime = QDateTime::currentDateTime();
        QDateTime startTime = endTime.addDays(-1);
        auto historyData = m_database->queryVitalSigns(startTime, endTime);
        m_historyChart->loadHistoryData(historyData);
    });
    
    connect(btnExport, &QPushButton::clicked, this, &ecg_app::onExportData);
    
    // 设置标签页
    QWidget* settingsTab = ui->tab_settings;
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsTab);
    
    QLabel* lblMqtt = new QLabel("MQTT服务器配置:", settingsTab);
    QLineEdit* editMqttHost = new QLineEdit(m_mqttHost, settingsTab);
    QSpinBox* spinMqttPort = new QSpinBox(settingsTab);
    spinMqttPort->setRange(1, 65535);
    spinMqttPort->setValue(m_mqttPort);
    
    QPushButton* btnSaveSettings = new QPushButton("保存设置", settingsTab);
    
    settingsLayout->addWidget(lblMqtt);
    settingsLayout->addWidget(new QLabel("主机:", settingsTab));
    settingsLayout->addWidget(editMqttHost);
    settingsLayout->addWidget(new QLabel("端口:", settingsTab));
    settingsLayout->addWidget(spinMqttPort);
    settingsLayout->addWidget(btnSaveSettings);
    settingsLayout->addStretch();
    
    connect(btnSaveSettings, &QPushButton::clicked, this, [this, editMqttHost, spinMqttPort]() {
        m_mqttHost = editMqttHost->text();
        m_mqttPort = spinMqttPort->value();
        saveSettings();
        QMessageBox::information(this, "设置", "设置已保存");
    });
    
    // 设置状态栏
    statusBar()->showMessage("就绪");
}

void ecg_app::connectSignals() {
#ifndef NO_MQTT_SUPPORT
    // MQTT信号
    connect(m_mqttClient, &MqttClientManager::vitalSignReceived,
            this, &ecg_app::onVitalSignReceived);
    connect(m_mqttClient, &MqttClientManager::alarmReceived,
            this, &ecg_app::onAlarmReceived);
    connect(m_mqttClient, &MqttClientManager::connectionStateChanged,
            this, &ecg_app::onMqttConnected);
#endif
    
    // 云同步信号
    connect(m_cloudSync, &CloudSyncManager::uploadCompleted,
            this, &ecg_app::onUploadCompleted);
    connect(m_cloudSync, &CloudSyncManager::downloadCompleted,
            this, &ecg_app::onDownloadCompleted);
    
    // 菜单动作
    connect(ui->actionConnect, &QAction::triggered, this, &ecg_app::onConnectDevice);
    connect(ui->actionDisconnect, &QAction::triggered, this, &ecg_app::onDisconnectDevice);
    connect(ui->actionExport, &QAction::triggered, this, &ecg_app::onExportData);
    connect(ui->actionSync, &QAction::triggered, this, &ecg_app::onSyncToCloud);
    connect(ui->actionShare, &QAction::triggered, this, &ecg_app::onShareData);
    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "关于", "ECG监护系统 v1.0\n暂未启用MQTT（模块未安装），其他功能正常");
    });
}

void ecg_app::loadSettings() {
    QSettings settings("ECGApp", "ECGMonitor");
    m_mqttHost = settings.value("mqtt/host", "localhost").toString();
    m_mqttPort = settings.value("mqtt/port", 1883).toInt();
    m_cloudServerUrl = settings.value("cloud/server", "https://ecg-cloud.com").toString();
}

void ecg_app::saveSettings() {
    QSettings settings("ECGApp", "ECGMonitor");
    settings.setValue("mqtt/host", m_mqttHost);
    settings.setValue("mqtt/port", m_mqttPort);
    settings.setValue("cloud/server", m_cloudServerUrl);
}

void ecg_app::onVitalSignReceived(const VitalSignData& data) {
    // 更新显示
    m_vitalSignPanel->updateVitalSigns(data);
    
    // 更新图表
    m_realtimeChart->addTrendPoint(data);
    
    // 显示ECG波形
    if (!data.ecgSignal.isEmpty()) {
        m_ecgWaveform->addECGData(data.ecgSignal);
    }
    
    // 保存到数据库
    m_database->saveVitalSign(data);
    
    // 上传到云端（队列）
    m_cloudSync->uploadVitalSign(data);
    
    statusBar()->showMessage(QString("收到数据: 体温=%1°C 心率=%2bpm 血氧=%3%")
                             .arg(data.temperature, 0, 'f', 1)
                             .arg(data.heartRate)
                             .arg(data.oxygenSaturation));
}

void ecg_app::onAlarmReceived(const AlarmInfo& alarm) {
    // 显示报警
    m_vitalSignPanel->showAlarm(alarm);
    
    // 保存到数据库
    m_database->saveAlarm(alarm);
    
    // 上传到云端
    m_cloudSync->uploadAlarm(alarm);
    
    // 显示提示框
    QMessageBox::warning(this, "报警", alarm.message);
    
    statusBar()->showMessage("报警: " + alarm.message, 5000);
}

void ecg_app::onMqttConnected(bool connected) {
    if (connected) {
        statusBar()->showMessage("MQTT已连接");
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
    } else {
        statusBar()->showMessage("MQTT已断开");
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
    }
}

void ecg_app::onConnectDevice() {
#ifdef NO_MQTT_SUPPORT
    QMessageBox::information(this, "提示", "MQTT功能未启用\n\n请使用Qt Maintenance Tool安装QtMqtt模块");
    return;
#else
    m_mqttClient->connectToHost(m_mqttHost, m_mqttPort);
    statusBar()->showMessage("正在连接...");
#endif
}

void ecg_app::onDisconnectDevice() {
#ifndef NO_MQTT_SUPPORT
    m_mqttClient->disconnectFromHost();
#endif
}

void ecg_app::onExportData() {
    QString fileName = QFileDialog::getSaveFileName(this, "导出数据", "", "CSV文件 (*.csv)");
    if (fileName.isEmpty()) return;
    
    QDateTime endTime = QDateTime::currentDateTime();
    QDateTime startTime = endTime.addDays(-7); // 导出最近7天
    
    if (m_database->exportToCSV(fileName, startTime, endTime)) {
        QMessageBox::information(this, "导出", "数据导出成功");
    } else {
        QMessageBox::warning(this, "导出", "数据导出失败");
    }
}

void ecg_app::onSyncToCloud() {
    if (!m_cloudSync->isLoggedIn()) {
        bool ok;
        QString username = QInputDialog::getText(this, "登录", "用户名:", QLineEdit::Normal, "", &ok);
        if (!ok || username.isEmpty()) return;
        
        QString password = QInputDialog::getText(this, "登录", "密码:", QLineEdit::Password, "", &ok);
        if (!ok) return;
        
        m_cloudSync->login(username, password);
    }
    
    m_cloudSync->syncNow();
}

void ecg_app::onShareData() {
    bool ok;
    QString email = QInputDialog::getText(this, "分享数据", "接收者邮箱:", QLineEdit::Normal, "", &ok);
    if (!ok || email.isEmpty()) return;
    
    QDateTime endTime = QDateTime::currentDateTime();
    QDateTime startTime = endTime.addDays(-1);
    
    m_cloudSync->shareDataWithUser(email, startTime, endTime);
    QMessageBox::information(this, "分享", "分享链接已发送到邮箱");
}

void ecg_app::onUploadCompleted(bool success) {
    if (success) {
        statusBar()->showMessage("数据同步成功", 3000);
    } else {
        statusBar()->showMessage("数据同步失败", 3000);
    }
}

void ecg_app::onDownloadCompleted(const QVector<VitalSignData>& data) {
    statusBar()->showMessage(QString("下载了%1条记录").arg(data.size()), 3000);
    
    // 保存到本地数据库
    m_database->saveVitalSignBatch(data);
}
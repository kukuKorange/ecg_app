#pragma once
#include "ui_ecg_app.h"
#include <QMainWindow>

#ifndef NO_MQTT_SUPPORT
#include "MqttClientManager.h"
#endif

#include "DatabaseManager.h"
#include "ChartWidget.h"
#include "CloudSyncManager.h"

class ecg_app : public QMainWindow {
    Q_OBJECT
    
public:
    ecg_app(QWidget* parent = nullptr);
    ~ecg_app();

private slots:
    // MQTT相关
    void onVitalSignReceived(const VitalSignData& data);
    void onAlarmReceived(const AlarmInfo& alarm);
    void onMqttConnected(bool connected);
    
    // 菜单操作
    void onConnectDevice();
    void onDisconnectDevice();
    void onExportData();
    void onSyncToCloud();
    void onShareData();
    
    // 云同步
    void onUploadCompleted(bool success);
    void onDownloadCompleted(const QVector<VitalSignData>& data);

private:
    Ui_ecg_app* ui;
    
    // 核心模块
#ifndef NO_MQTT_SUPPORT
    MqttClientManager* m_mqttClient;
#endif
    DatabaseManager* m_database;
    CloudSyncManager* m_cloudSync;
    
    // UI组件
    ChartWidget* m_realtimeChart;
    ECGWaveformWidget* m_ecgWaveform;
    VitalSignPanel* m_vitalSignPanel;
    ChartWidget* m_historyChart;
    
    // 初始化函数
    void initializeModules();
    void setupUI();
    void connectSignals();
    void loadSettings();
    void saveSettings();
    
    // 设置项
    QString m_mqttHost;
    quint16 m_mqttPort;
    QString m_cloudServerUrl;
};
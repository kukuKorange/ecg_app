#pragma once
#include <QObject>
#include "VitalSignData.h"

#ifndef NO_MQTT_SUPPORT
#include <QMqttClient>
#include <QMqttSubscription>

class MqttClientManager : public QObject {
    Q_OBJECT

public:
    explicit MqttClientManager(QObject* parent = nullptr);
    ~MqttClientManager();

    // 连接到MQTT服务器
    void connectToHost(const QString& host, quint16 port = 1883);
    
    // 设置认证信息
    void setAuthentication(const QString& username, const QString& password);
    
    // 订阅主题
    void subscribeTopic(const QString& topic);
    
    // 取消订阅
    void unsubscribeTopic(const QString& topic);
    
    // 发布消息
    void publishMessage(const QString& topic, const QByteArray& message);
    
    // 断开连接
    void disconnectFromHost();
    
    // 获取连接状态
    bool isConnected() const;

signals:
    // 接收到生理数据
    void vitalSignReceived(const VitalSignData& data);
    
    // 接收到报警信息
    void alarmReceived(const AlarmInfo& alarm);
    
    // 连接状态改变
    void connectionStateChanged(bool connected);
    
    // 错误信号
    void errorOccurred(const QString& error);

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(const QByteArray& message, const QMqttTopicName& topic);
    void onStateChanged(QMqttClient::ClientState state);
    void onErrorOccurred(QMqttClient::ClientError error);

private:
    QMqttClient* m_client;
    QMap<QString, QMqttSubscription*> m_subscriptions;
    
    // 解析接收到的数据
    void parseVitalSignData(const QByteArray& data);
    void parseAlarmData(const QByteArray& data);
};

#else // NO_MQTT_SUPPORT

// 当 MQTT 支持被禁用时，提供一个空的存根类
class MqttClientManager : public QObject {
    Q_OBJECT

public:
    explicit MqttClientManager(QObject* parent = nullptr) : QObject(parent) {}
    ~MqttClientManager() {}

    void connectToHost(const QString& /*host*/, quint16 /*port*/ = 1883) {}
    void setAuthentication(const QString& /*username*/, const QString& /*password*/) {}
    void subscribeTopic(const QString& /*topic*/) {}
    void unsubscribeTopic(const QString& /*topic*/) {}
    void publishMessage(const QString& /*topic*/, const QByteArray& /*message*/) {}
    void disconnectFromHost() {}
    bool isConnected() const { return false; }

signals:
    void vitalSignReceived(const VitalSignData& data);
    void alarmReceived(const AlarmInfo& alarm);
    void connectionStateChanged(bool connected);
    void errorOccurred(const QString& error);
};

#endif // NO_MQTT_SUPPORT

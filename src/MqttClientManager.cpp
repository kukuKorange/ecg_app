#ifndef NO_MQTT_SUPPORT

#include "MqttClientManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

MqttClientManager::MqttClientManager(QObject* parent)
    : QObject(parent)
    , m_client(new QMqttClient(this))
{
    connect(m_client, &QMqttClient::connected, this, &MqttClientManager::onConnected);
    connect(m_client, &QMqttClient::disconnected, this, &MqttClientManager::onDisconnected);
    connect(m_client, &QMqttClient::messageReceived, this, &MqttClientManager::onMessageReceived);
    connect(m_client, &QMqttClient::stateChanged, this, &MqttClientManager::onStateChanged);
    connect(m_client, &QMqttClient::errorChanged, this, &MqttClientManager::onErrorOccurred);
}

MqttClientManager::~MqttClientManager() {
    if (m_client->state() == QMqttClient::Connected) {
        m_client->disconnectFromHost();
    }
}

void MqttClientManager::connectToHost(const QString& host, quint16 port) {
    m_client->setHostname(host);
    m_client->setPort(port);
    m_client->connectToHost();
}

void MqttClientManager::setAuthentication(const QString& username, const QString& password) {
    m_client->setUsername(username);
    m_client->setPassword(password);
}

void MqttClientManager::subscribeTopic(const QString& topic) {
    if (m_client->state() == QMqttClient::Connected) {
        auto subscription = m_client->subscribe(topic, 1);
        if (subscription) {
            m_subscriptions[topic] = subscription;
            qDebug() << "Subscribed to topic:" << topic;
        }
    }
}

void MqttClientManager::unsubscribeTopic(const QString& topic) {
    if (m_subscriptions.contains(topic)) {
        m_subscriptions[topic]->unsubscribe();
        m_subscriptions.remove(topic);
    }
}

void MqttClientManager::publishMessage(const QString& topic, const QByteArray& message) {
    if (m_client->state() == QMqttClient::Connected) {
        m_client->publish(topic, message, 1);
    }
}

void MqttClientManager::disconnectFromHost() {
    m_client->disconnectFromHost();
}

bool MqttClientManager::isConnected() const {
    return m_client->state() == QMqttClient::Connected;
}

void MqttClientManager::onConnected() {
    qDebug() << "Connected to MQTT broker";
    emit connectionStateChanged(true);
    
    // 自动订阅数据和报警主题
    subscribeTopic("ecg/vitalsign");
    subscribeTopic("ecg/alarm");
}

void MqttClientManager::onDisconnected() {
    qDebug() << "Disconnected from MQTT broker";
    emit connectionStateChanged(false);
}

void MqttClientManager::onMessageReceived(const QByteArray& message, const QMqttTopicName& topic) {
    QString topicName = topic.name();
    qDebug() << "Message received on topic:" << topicName;
    
    if (topicName == "ecg/vitalsign") {
        parseVitalSignData(message);
    } else if (topicName == "ecg/alarm") {
        parseAlarmData(message);
    }
}

void MqttClientManager::onStateChanged(QMqttClient::ClientState state) {
    qDebug() << "MQTT client state changed:" << state;
}

void MqttClientManager::onErrorOccurred(QMqttClient::ClientError error) {
    QString errorMsg;
    switch (error) {
        case QMqttClient::NoError:
            return;
        case QMqttClient::InvalidProtocolVersion:
            errorMsg = "Invalid protocol version";
            break;
        case QMqttClient::IdRejected:
            errorMsg = "Client ID rejected";
            break;
        case QMqttClient::ServerUnavailable:
            errorMsg = "Server unavailable";
            break;
        case QMqttClient::BadUsernameOrPassword:
            errorMsg = "Bad username or password";
            break;
        case QMqttClient::NotAuthorized:
            errorMsg = "Not authorized";
            break;
        default:
            errorMsg = "Unknown error";
            break;
    }
    
    qWarning() << "MQTT error:" << errorMsg;
    emit errorOccurred(errorMsg);
}

void MqttClientManager::parseVitalSignData(const QByteArray& data) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON format for vital sign data";
        return;
    }
    
    VitalSignData vitalSign = VitalSignData::fromJson(doc.object());
    
    if (vitalSign.isValid()) {
        emit vitalSignReceived(vitalSign);
    } else {
        qWarning() << "Received invalid vital sign data";
    }
}

void MqttClientManager::parseAlarmData(const QByteArray& data) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON format for alarm data";
        return;
    }
    
    AlarmInfo alarm = AlarmInfo::fromJson(doc.object());
    emit alarmReceived(alarm);
}

#endif // NO_MQTT_SUPPORT

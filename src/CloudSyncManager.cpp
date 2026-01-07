#include "CloudSyncManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QHttpMultiPart>
#include <QCryptographicHash>
#include <QUuid>
#include <QDebug>

CloudSyncManager::CloudSyncManager(QObject* parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_autoSyncTimer(new QTimer(this))
    , m_isLoggedIn(false)
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &CloudSyncManager::onUploadFinished);
    
    connect(m_autoSyncTimer, &QTimer::timeout,
            this, &CloudSyncManager::onAutoSyncTriggered);
}

CloudSyncManager::~CloudSyncManager() {
    m_autoSyncTimer->stop();
}

void CloudSyncManager::setServerUrl(const QString& url) {
    m_serverUrl = url;
}

void CloudSyncManager::setApiKey(const QString& apiKey) {
    m_apiKey = apiKey;
}

void CloudSyncManager::setDeviceId(const QString& deviceId) {
    m_deviceId = deviceId;
}

void CloudSyncManager::login(const QString& username, const QString& password) {
    QJsonObject loginData;
    loginData["username"] = username;
    loginData["password"] = password;
    loginData["deviceId"] = m_deviceId;
    
    QNetworkRequest request = buildRequest("/api/auth/login");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(loginData);
    QNetworkReply* reply = m_networkManager->post(request, doc.toJson());
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject obj = doc.object();
            
            if (obj.contains("token")) {
                m_authToken = obj["token"].toString();
                m_isLoggedIn = true;
                emit loginStateChanged(true);
                qDebug() << "Login successful";
            }
        } else {
            emit errorOccurred("登录失败: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

void CloudSyncManager::logout() {
    m_authToken.clear();
    m_isLoggedIn = false;
    emit loginStateChanged(false);
}

void CloudSyncManager::uploadVitalSign(const VitalSignData& data) {
    if (!m_isLoggedIn) {
        addToUploadQueue(data);
        return;
    }
    
    QJsonObject jsonData = data.toJson();
    jsonData["deviceId"] = m_deviceId;
    
    QNetworkRequest request = buildRequest("/api/vitalsign/upload");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(jsonData);
    m_networkManager->post(request, doc.toJson());
    
    emit syncStatusChanged("正在上传数据...");
}

void CloudSyncManager::uploadVitalSignBatch(const QVector<VitalSignData>& dataList) {
    if (!m_isLoggedIn) {
        for (const auto& data : dataList) {
            addToUploadQueue(data);
        }
        return;
    }
    
    QJsonArray dataArray;
    for (const auto& data : dataList) {
        dataArray.append(data.toJson());
    }
    
    QJsonObject batchData;
    batchData["deviceId"] = m_deviceId;
    batchData["data"] = dataArray;
    
    QNetworkRequest request = buildRequest("/api/vitalsign/batch");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(batchData);
    m_networkManager->post(request, doc.toJson());
    
    emit syncStatusChanged(QString("正在批量上传 %1 条数据...").arg(dataList.size()));
}

void CloudSyncManager::uploadAlarm(const AlarmInfo& alarm) {
    if (!m_isLoggedIn) return;
    
    QJsonObject jsonData = alarm.toJson();
    jsonData["deviceId"] = m_deviceId;
    
    QNetworkRequest request = buildRequest("/api/alarm/upload");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(jsonData);
    m_networkManager->post(request, doc.toJson());
}

void CloudSyncManager::enableAutoSync(bool enable, int intervalMinutes) {
    if (enable) {
        m_autoSyncTimer->start(intervalMinutes * 60 * 1000);
        qDebug() << "Auto sync enabled, interval:" << intervalMinutes << "minutes";
    } else {
        m_autoSyncTimer->stop();
        qDebug() << "Auto sync disabled";
    }
}

void CloudSyncManager::syncNow() {
    if (!m_isLoggedIn) {
        emit errorOccurred("请先登录");
        return;
    }
    
    emit syncStatusChanged("开始同步...");
    processUploadQueue();
}

void CloudSyncManager::downloadHistoryData(const QDateTime& startTime, const QDateTime& endTime) {
    if (!m_isLoggedIn) {
        emit errorOccurred("请先登录");
        return;
    }
    
    QUrlQuery query;
    query.addQueryItem("deviceId", m_deviceId);
    query.addQueryItem("startTime", startTime.toString(Qt::ISODate));
    query.addQueryItem("endTime", endTime.toString(Qt::ISODate));
    
    QUrl url(m_serverUrl + "/api/vitalsign/history");
    url.setQuery(query);
    
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_authToken).toUtf8());
    request.setRawHeader("X-API-Key", m_apiKey.toUtf8());
    
    QNetworkReply* reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onDownloadFinished(reply);
    });
    
    emit syncStatusChanged("正在下载历史数据...");
}

void CloudSyncManager::shareDataWithUser(const QString& recipientEmail,
                                         const QDateTime& startTime,
                                         const QDateTime& endTime) {
    if (!m_isLoggedIn) return;
    
    QJsonObject shareData;
    shareData["deviceId"] = m_deviceId;
    shareData["recipientEmail"] = recipientEmail;
    shareData["startTime"] = startTime.toString(Qt::ISODate);
    shareData["endTime"] = endTime.toString(Qt::ISODate);
    shareData["validDays"] = 7;
    
    QNetworkRequest request = buildRequest("/api/share/create");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(shareData);
    QNetworkReply* reply = m_networkManager->post(request, doc.toJson());
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QString shareLink = doc.object()["shareLink"].toString();
            emit syncStatusChanged("数据已分享: " + shareLink);
        } else {
            emit errorOccurred("分享失败: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

void CloudSyncManager::fetchSharedData(const QString& shareToken) {
    QNetworkRequest request = buildRequest("/api/share/data?token=" + shareToken);
    
    QNetworkReply* reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onDownloadFinished(reply);
    });
}

QNetworkRequest CloudSyncManager::buildRequest(const QString& endpoint) {
    QUrl url(m_serverUrl + endpoint);
    QNetworkRequest request(url);
    
    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(m_authToken).toUtf8());
    }
    
    if (!m_apiKey.isEmpty()) {
        request.setRawHeader("X-API-Key", m_apiKey.toUtf8());
    }
    
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("User-Agent", "ECG-App/1.0");
    
    return request;
}

void CloudSyncManager::onUploadFinished(QNetworkReply* reply) {
    handleUploadResponse(reply);
    reply->deleteLater();
}

void CloudSyncManager::onDownloadFinished(QNetworkReply* reply) {
    handleDownloadResponse(reply);
    reply->deleteLater();
}

void CloudSyncManager::handleUploadResponse(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        emit uploadCompleted(true);
        emit syncStatusChanged("上传成功");
        qDebug() << "Data uploaded successfully";
    } else {
        emit uploadCompleted(false);
        emit errorOccurred("上传失败: " + reply->errorString());
        qWarning() << "Upload failed:" << reply->errorString();
    }
}

void CloudSyncManager::handleDownloadResponse(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonArray dataArray = doc.object()["data"].toArray();
        
        QVector<VitalSignData> historyData;
        for (const QJsonValue& value : dataArray) {
            historyData.append(VitalSignData::fromJson(value.toObject()));
        }
        
        emit downloadCompleted(historyData);
        emit syncStatusChanged(QString("下载完成: %1 条记录").arg(historyData.size()));
    } else {
        emit errorOccurred("下载失败: " + reply->errorString());
    }
}

void CloudSyncManager::addToUploadQueue(const VitalSignData& data) {
    m_uploadQueue.append(data);
}

void CloudSyncManager::processUploadQueue() {
    if (m_uploadQueue.isEmpty()) {
        emit syncStatusChanged("无待上传数据");
        return;
    }
    
    uploadVitalSignBatch(m_uploadQueue);
    m_uploadQueue.clear();
}

void CloudSyncManager::onAutoSyncTriggered() {
    if (m_isLoggedIn) {
        syncNow();
    }
}

// ==================== DataShareManager ====================

DataShareManager::DataShareManager(QObject* parent)
    : QObject(parent)
{
}

QString DataShareManager::generateShareToken(const QString& userId,
                                             const QDateTime& startTime,
                                             const QDateTime& endTime,
                                             int validDays) {
    QString tokenData = QString("%1_%2_%3_%4")
                           .arg(userId)
                           .arg(startTime.toString(Qt::ISODate))
                           .arg(endTime.toString(Qt::ISODate))
                           .arg(QDateTime::currentDateTime().toString(Qt::ISODate));
    
    QByteArray hash = QCryptographicHash::hash(tokenData.toUtf8(), QCryptographicHash::Sha256);
    QString token = hash.toHex().left(32);
    
    m_activeTokens[token] = QDateTime::currentDateTime().addDays(validDays);
    
    return token;
}

bool DataShareManager::validateShareToken(const QString& token) {
    if (!m_activeTokens.contains(token)) {
        return false;
    }
    
    QDateTime expiryDate = m_activeTokens[token];
    if (QDateTime::currentDateTime() > expiryDate) {
        m_activeTokens.remove(token);
        return false;
    }
    
    return true;
}

QString DataShareManager::getShareLink(const QString& token) {
    return QString("https://ecg-app.com/share?token=%1").arg(token);
}

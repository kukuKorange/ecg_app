#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include "VitalSignData.h"

class CloudSyncManager : public QObject {
    Q_OBJECT

public:
    explicit CloudSyncManager(QObject* parent = nullptr);
    ~CloudSyncManager();

    // 配置服务器信息
    void setServerUrl(const QString& url);
    void setApiKey(const QString& apiKey);
    void setDeviceId(const QString& deviceId);
    
    // 用户认证
    void login(const QString& username, const QString& password);
    void logout();
    bool isLoggedIn() const { return m_isLoggedIn; }
    
    // 上传数据到云端
    void uploadVitalSign(const VitalSignData& data);
    void uploadVitalSignBatch(const QVector<VitalSignData>& dataList);
    void uploadAlarm(const AlarmInfo& alarm);
    
    // 自动同步设置
    void enableAutoSync(bool enable, int intervalMinutes = 5);
    
    // 手动触发同步
    void syncNow();
    
    // 从云端下载数据
    void downloadHistoryData(const QDateTime& startTime, const QDateTime& endTime);
    
    // 分享数据给家属/医生
    void shareDataWithUser(const QString& recipientEmail, 
                          const QDateTime& startTime, 
                          const QDateTime& endTime);
    
    // 获取分享的数据（远程访问）
    void fetchSharedData(const QString& shareToken);

signals:
    // 上传完成
    void uploadCompleted(bool success);
    
    // 下载完成
    void downloadCompleted(const QVector<VitalSignData>& data);
    
    // 同步状态
    void syncStatusChanged(const QString& status);
    
    // 错误信号
    void errorOccurred(const QString& error);
    
    // 登录状态变化
    void loginStateChanged(bool loggedIn);

private slots:
    void onUploadFinished(QNetworkReply* reply);
    void onDownloadFinished(QNetworkReply* reply);
    void onAutoSyncTriggered();

private:
    QNetworkAccessManager* m_networkManager;
    QTimer* m_autoSyncTimer;
    
    QString m_serverUrl;
    QString m_apiKey;
    QString m_deviceId;
    QString m_authToken;
    bool m_isLoggedIn;
    
    // 待上传队列
    QVector<VitalSignData> m_uploadQueue;
    
    // 构建HTTP请求
    QNetworkRequest buildRequest(const QString& endpoint);
    
    // 处理响应
    void handleUploadResponse(QNetworkReply* reply);
    void handleDownloadResponse(QNetworkReply* reply);
    
    // 添加到上传队列
    void addToUploadQueue(const VitalSignData& data);
    void processUploadQueue();
};

// 数据分享管理器
class DataShareManager : public QObject {
    Q_OBJECT

public:
    explicit DataShareManager(QObject* parent = nullptr);
    
    // 生成分享令牌
    QString generateShareToken(const QString& userId, 
                              const QDateTime& startTime,
                              const QDateTime& endTime,
                              int validDays = 7);
    
    // 验证分享令牌
    bool validateShareToken(const QString& token);
    
    // 获取分享链接
    QString getShareLink(const QString& token);

signals:
    void shareCreated(const QString& shareLink);
    void shareRevoked(const QString& token);

private:
    QMap<QString, QDateTime> m_activeTokens;
};

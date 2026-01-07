#pragma once
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "VitalSignData.h"

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();

    // 初始化数据库
    bool initialize(const QString& dbPath = "");
    
    // 保存生理数据
    bool saveVitalSign(const VitalSignData& data);
    
    // 批量保存
    bool saveVitalSignBatch(const QVector<VitalSignData>& dataList);
    
    // 保存报警信息
    bool saveAlarm(const AlarmInfo& alarm);
    
    // 查询历史数据
    QVector<VitalSignData> queryVitalSigns(const QDateTime& startTime, 
                                           const QDateTime& endTime,
                                           int limit = 1000);
    
    // 查询报警历史
    QVector<AlarmInfo> queryAlarms(const QDateTime& startTime,
                                   const QDateTime& endTime,
                                   int limit = 100);
    
    // 获取最新数据
    VitalSignData getLatestVitalSign();
    
    // 删除旧数据（数据清理）
    bool deleteOldData(int daysToKeep = 30);
    
    // 获取统计信息
    struct Statistics {
        double avgTemperature;
        double avgHeartRate;
        double avgOxygen;
        int totalRecords;
    };
    Statistics getStatistics(const QDateTime& startTime, const QDateTime& endTime);
    
    // 导出数据到CSV
    bool exportToCSV(const QString& filePath, 
                     const QDateTime& startTime, 
                     const QDateTime& endTime);

signals:
    void databaseError(const QString& error);

private:
    QSqlDatabase m_db;
    
    // 创建数据表
    bool createTables();
    
    // 检查数据库连接
    bool checkConnection();
};

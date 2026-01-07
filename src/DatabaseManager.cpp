#include "DatabaseManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent)
{
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::initialize(const QString& dbPath) {
    QString path = dbPath;
    if (path.isEmpty()) {
        // 使用应用数据目录
        QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(appDataPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        path = appDataPath + "/ecg_data.db";
    }
    
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    
    if (!m_db.open()) {
        emit databaseError("无法打开数据库: " + m_db.lastError().text());
        return false;
    }
    
    qDebug() << "Database opened at:" << path;
    return createTables();
}

bool DatabaseManager::createTables() {
    QSqlQuery query(m_db);
    
    // 创建生理数据表
    QString createVitalSignTable = R"(
        CREATE TABLE IF NOT EXISTS vital_signs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME NOT NULL,
            temperature REAL,
            oxygen_saturation INTEGER,
            heart_rate INTEGER,
            ecg_signal TEXT,
            INDEX idx_timestamp (timestamp)
        )
    )";
    
    if (!query.exec(createVitalSignTable)) {
        emit databaseError("创建vital_signs表失败: " + query.lastError().text());
        return false;
    }
    
    // 创建报警表
    QString createAlarmTable = R"(
        CREATE TABLE IF NOT EXISTS alarms (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME NOT NULL,
            type INTEGER,
            message TEXT,
            severity INTEGER,
            INDEX idx_alarm_timestamp (timestamp)
        )
    )";
    
    if (!query.exec(createAlarmTable)) {
        emit databaseError("创建alarms表失败: " + query.lastError().text());
        return false;
    }
    
    qDebug() << "Database tables created successfully";
    return true;
}

bool DatabaseManager::checkConnection() {
    if (!m_db.isOpen()) {
        emit databaseError("数据库未连接");
        return false;
    }
    return true;
}

bool DatabaseManager::saveVitalSign(const VitalSignData& data) {
    if (!checkConnection()) return false;
    
    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO vital_signs (timestamp, temperature, oxygen_saturation, heart_rate, ecg_signal)
        VALUES (:timestamp, :temperature, :oxygen_saturation, :heart_rate, :ecg_signal)
    )");
    
    query.bindValue(":timestamp", data.timestamp);
    query.bindValue(":temperature", data.temperature);
    query.bindValue(":oxygen_saturation", data.oxygenSaturation);
    query.bindValue(":heart_rate", data.heartRate);
    
    // 将ECG信号数组转换为JSON字符串存储
    QJsonDocument doc(data.toJson());
    query.bindValue(":ecg_signal", QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
    
    if (!query.exec()) {
        emit databaseError("保存数据失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

bool DatabaseManager::saveVitalSignBatch(const QVector<VitalSignData>& dataList) {
    if (!checkConnection()) return false;
    
    m_db.transaction();
    
    for (const auto& data : dataList) {
        if (!saveVitalSign(data)) {
            m_db.rollback();
            return false;
        }
    }
    
    return m_db.commit();
}

bool DatabaseManager::saveAlarm(const AlarmInfo& alarm) {
    if (!checkConnection()) return false;
    
    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO alarms (timestamp, type, message, severity)
        VALUES (:timestamp, :type, :message, :severity)
    )");
    
    query.bindValue(":timestamp", alarm.timestamp);
    query.bindValue(":type", static_cast<int>(alarm.type));
    query.bindValue(":message", alarm.message);
    query.bindValue(":severity", alarm.severity);
    
    if (!query.exec()) {
        emit databaseError("保存报警信息失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

QVector<VitalSignData> DatabaseManager::queryVitalSigns(const QDateTime& startTime,
                                                        const QDateTime& endTime,
                                                        int limit) {
    QVector<VitalSignData> result;
    if (!checkConnection()) return result;
    
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT timestamp, temperature, oxygen_saturation, heart_rate, ecg_signal
        FROM vital_signs
        WHERE timestamp BETWEEN :start AND :end
        ORDER BY timestamp DESC
        LIMIT :limit
    )");
    
    query.bindValue(":start", startTime);
    query.bindValue(":end", endTime);
    query.bindValue(":limit", limit);
    
    if (!query.exec()) {
        emit databaseError("查询数据失败: " + query.lastError().text());
        return result;
    }
    
    while (query.next()) {
        VitalSignData data;
        data.timestamp = query.value(0).toDateTime();
        data.temperature = query.value(1).toDouble();
        data.oxygenSaturation = query.value(2).toInt();
        data.heartRate = query.value(3).toInt();
        
        // 解析ECG信号
        QString ecgJson = query.value(4).toString();
        QJsonDocument doc = QJsonDocument::fromJson(ecgJson.toUtf8());
        if (doc.isObject()) {
            data = VitalSignData::fromJson(doc.object());
        }
        
        result.append(data);
    }
    
    return result;
}

QVector<AlarmInfo> DatabaseManager::queryAlarms(const QDateTime& startTime,
                                                const QDateTime& endTime,
                                                int limit) {
    QVector<AlarmInfo> result;
    if (!checkConnection()) return result;
    
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT timestamp, type, message, severity
        FROM alarms
        WHERE timestamp BETWEEN :start AND :end
        ORDER BY timestamp DESC
        LIMIT :limit
    )");
    
    query.bindValue(":start", startTime);
    query.bindValue(":end", endTime);
    query.bindValue(":limit", limit);
    
    if (!query.exec()) {
        emit databaseError("查询报警失败: " + query.lastError().text());
        return result;
    }
    
    while (query.next()) {
        AlarmInfo alarm;
        alarm.timestamp = query.value(0).toDateTime();
        alarm.type = static_cast<AlarmInfo::AlarmType>(query.value(1).toInt());
        alarm.message = query.value(2).toString();
        alarm.severity = query.value(3).toInt();
        result.append(alarm);
    }
    
    return result;
}

VitalSignData DatabaseManager::getLatestVitalSign() {
    VitalSignData data;
    if (!checkConnection()) return data;
    
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT timestamp, temperature, oxygen_saturation, heart_rate, ecg_signal
        FROM vital_signs
        ORDER BY timestamp DESC
        LIMIT 1
    )");
    
    if (query.exec() && query.next()) {
        data.timestamp = query.value(0).toDateTime();
        data.temperature = query.value(1).toDouble();
        data.oxygenSaturation = query.value(2).toInt();
        data.heartRate = query.value(3).toInt();
        
        QString ecgJson = query.value(4).toString();
        QJsonDocument doc = QJsonDocument::fromJson(ecgJson.toUtf8());
        if (doc.isObject()) {
            data = VitalSignData::fromJson(doc.object());
        }
    }
    
    return data;
}

bool DatabaseManager::deleteOldData(int daysToKeep) {
    if (!checkConnection()) return false;
    
    QDateTime cutoffDate = QDateTime::currentDateTime().addDays(-daysToKeep);
    
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM vital_signs WHERE timestamp < :cutoff");
    query.bindValue(":cutoff", cutoffDate);
    
    if (!query.exec()) {
        emit databaseError("删除旧数据失败: " + query.lastError().text());
        return false;
    }
    
    qDebug() << "Deleted" << query.numRowsAffected() << "old records";
    return true;
}

DatabaseManager::Statistics DatabaseManager::getStatistics(const QDateTime& startTime,
                                                           const QDateTime& endTime) {
    Statistics stats = {0, 0, 0, 0};
    if (!checkConnection()) return stats;
    
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT 
            AVG(temperature) as avg_temp,
            AVG(heart_rate) as avg_hr,
            AVG(oxygen_saturation) as avg_ox,
            COUNT(*) as total
        FROM vital_signs
        WHERE timestamp BETWEEN :start AND :end
    )");
    
    query.bindValue(":start", startTime);
    query.bindValue(":end", endTime);
    
    if (query.exec() && query.next()) {
        stats.avgTemperature = query.value(0).toDouble();
        stats.avgHeartRate = query.value(1).toDouble();
        stats.avgOxygen = query.value(2).toDouble();
        stats.totalRecords = query.value(3).toInt();
    }
    
    return stats;
}

bool DatabaseManager::exportToCSV(const QString& filePath,
                                  const QDateTime& startTime,
                                  const QDateTime& endTime) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit databaseError("无法创建CSV文件");
        return false;
    }
    
    QTextStream out(&file);
    // Qt6移除了setCodec，默认使用UTF-8
    out.setEncoding(QStringConverter::Utf8);
    
    // 写入表头
    out << "时间戳,体温(°C),血氧(%),心率(bpm)\n";
    
    // 查询数据
    auto data = queryVitalSigns(startTime, endTime, 10000);
    
    for (const auto& record : data) {
        out << record.timestamp.toString("yyyy-MM-dd hh:mm:ss") << ","
            << record.temperature << ","
            << record.oxygenSaturation << ","
            << record.heartRate << "\n";
    }
    
    file.close();
    qDebug() << "Exported" << data.size() << "records to" << filePath;
    return true;
}

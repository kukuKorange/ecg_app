#pragma once
#include <QString>
#include <QDateTime>
#include <QVector>
#include <QJsonObject>

// 生理信号数据结构
struct VitalSignData {
    QDateTime timestamp;        // 时间戳
    double temperature;         // 体温 (°C)
    int oxygenSaturation;      // 血氧饱和度 (%)
    int heartRate;             // 心率 (bpm)
    QVector<double> ecgSignal; // 心电图信号数组
    
    VitalSignData() 
        : timestamp(QDateTime::currentDateTime())
        , temperature(0.0)
        , oxygenSaturation(0)
        , heartRate(0)
    {}
    
    // 转换为JSON格式
    QJsonObject toJson() const;
    
    // 从JSON解析
    static VitalSignData fromJson(const QJsonObject& json);
    
    // 验证数据有效性
    bool isValid() const {
        return temperature >= 35.0 && temperature <= 42.0 &&
               oxygenSaturation >= 0 && oxygenSaturation <= 100 &&
               heartRate >= 30 && heartRate <= 220;
    }
};

// 报警信息结构
struct AlarmInfo {
    enum AlarmType {
        LowOxygen,          // 低血氧
        HighHeartRate,      // 心率过高
        LowHeartRate,       // 心率过低
        AbnormalTemperature,// 体温异常
        ECGAbnormal         // 心电异常
    };
    
    QDateTime timestamp;
    AlarmType type;
    QString message;
    int severity;  // 1-5, 5为最严重
    
    AlarmInfo() : timestamp(QDateTime::currentDateTime()), severity(1) {}
    
    QJsonObject toJson() const;
    static AlarmInfo fromJson(const QJsonObject& json);
};

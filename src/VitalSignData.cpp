#include "VitalSignData.h"
#include <QJsonArray>

QJsonObject VitalSignData::toJson() const {
    QJsonObject json;
    json["timestamp"] = timestamp.toString(Qt::ISODate);
    json["temperature"] = temperature;
    json["oxygenSaturation"] = oxygenSaturation;
    json["heartRate"] = heartRate;
    
    QJsonArray ecgArray;
    for (double value : ecgSignal) {
        ecgArray.append(value);
    }
    json["ecgSignal"] = ecgArray;
    
    return json;
}

VitalSignData VitalSignData::fromJson(const QJsonObject& json) {
    VitalSignData data;
    data.timestamp = QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate);
    data.temperature = json["temperature"].toDouble();
    data.oxygenSaturation = json["oxygenSaturation"].toInt();
    data.heartRate = json["heartRate"].toInt();
    
    QJsonArray ecgArray = json["ecgSignal"].toArray();
    for (const QJsonValue& value : ecgArray) {
        data.ecgSignal.append(value.toDouble());
    }
    
    return data;
}

QJsonObject AlarmInfo::toJson() const {
    QJsonObject json;
    json["timestamp"] = timestamp.toString(Qt::ISODate);
    json["type"] = static_cast<int>(type);
    json["message"] = message;
    json["severity"] = severity;
    return json;
}

AlarmInfo AlarmInfo::fromJson(const QJsonObject& json) {
    AlarmInfo alarm;
    alarm.timestamp = QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate);
    alarm.type = static_cast<AlarmType>(json["type"].toInt());
    alarm.message = json["message"].toString();
    alarm.severity = json["severity"].toInt();
    return alarm;
}

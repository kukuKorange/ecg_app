# ECG监护系统

## 项目简介

基于Qt开发的跨平台ECG（心电图）监护系统，支持Windows桌面和Android移动端。通过MQTT协议接收生理数据，实现数据存储、可视化、云同步和远程访问。

## 核心功能

### ✅ 四大核心模块

1. **MQTT通信模块** (`MqttClientManager`)
   - 连接MQTT Broker接收实时数据
   - 支持用户名/密码认证
   - 自动重连机制
   - 订阅主题：`ecg/vitalsign` 和 `ecg/alarm`

2. **数据存储模块** (`DatabaseManager`)
   - SQLite本地数据库
   - 自动存储所有生理数据和报警信息
   - 支持历史查询和统计分析
   - 数据导出为CSV格式

3. **数据可视化模块** (`ChartWidget`)
   - 实时心电图波形显示
   - 体温/心率/血氧趋势图
   - 历史数据回放
   - 多参数监护面板

4. **云同步模块** (`CloudSyncManager`)
   - RESTful API云端同步
   - 自动/手动上传数据
   - 数据分享给家属/医生
   - 远程数据访问

### 📊 监测数据类型

- **体温** (Temperature): 35-42°C
- **血氧饱和度** (SpO2): 0-100%
- **心率** (Heart Rate): 30-220 bpm
- **心电图信号** (ECG Signal): 实时波形数组

### ⚠️ 报警系统

- 低血氧报警
- 心率过高/过低
- 体温异常
- 心电异常
- 5级严重度分级

## 技术栈

- **开发框架**: Qt 6.x (C++17)
- **UI框架**: Qt Widgets + Qt Charts
- **通信协议**: MQTT (QtMqtt)
- **数据库**: SQLite (Qt SQL)
- **网络**: Qt Network
- **平台**: Windows / Android

## 项目结构

```
ecg_app/
├── src/
│   ├── main.cpp                    # 主入口
│   ├── ecg_app.h/cpp              # 主窗口
│   ├── VitalSignData.h/cpp        # 数据模型
│   ├── MqttClientManager.h/cpp    # MQTT通信
│   ├── DatabaseManager.h/cpp      # 数据库管理
│   ├── ChartWidget.h/cpp          # 图表组件
│   └── CloudSyncManager.h/cpp     # 云同步
├── android/
│   ├── AndroidManifest.xml        # Android配置
│   └── res/                       # 资源文件
├── scripts/
│   ├── build_debug.bat
│   └── build_release.bat
├── ecg_app.pro                    # Qt项目文件
├── android_config.pri             # Android配置
├── ANDROID_BUILD.md              # Android构建指南
└── README.md                      # 本文件
```

## 快速开始

### 桌面版 (Windows)

#### 前置要求
- Qt 6.x (包含 QtMqtt, QtCharts)
- MSVC 2019+ 或 MinGW
- CMake 3.16+ (可选)

#### 编译运行
```bash
# 使用Qt Creator
1. 打开 ecg_app.pro
2. 配置Kit (MSVC/MinGW)
3. 点击运行

# 或使用命令行
qmake ecg_app.pro
nmake  # MSVC
# 或 mingw32-make  # MinGW

# 运行
./ecg_app.exe
```

### Android版

详细说明请查看 [ANDROID_BUILD.md](ANDROID_BUILD.md)

#### 简要步骤
1. 安装 Qt for Android
2. 配置 Android SDK/NDK
3. Qt Creator 选择 Android Kit
4. 构建 > 部署
5. 生成APK位于 `android-build/outputs/apk/`

## 使用说明

### 1. 连接MQTT设备

1. 点击菜单 **文件 > 设置**
2. 配置MQTT服务器地址和端口
3. 点击 **文件 > 连接设备**
4. 状态栏显示"MQTT已连接"

### 2. 实时监护

- 切换到"实时监护"标签页
- 左侧显示当前数值（体温、心率、血氧）
- 右上方显示实时心电图波形
- 右下方显示趋势图

### 3. 历史查询

1. 切换到"历史查询"标签页
2. 点击"查询"按钮（默认显示最近24小时）
3. 查看历史趋势图
4. 点击"导出CSV"保存数据

### 4. 云同步

1. 点击菜单 **数据 > 同步到云端**
2. 首次使用需要登录
3. 数据自动上传（每5分钟）

### 5. 数据分享

1. 点击菜单 **数据 > 分享数据**
2. 输入接收者邮箱
3. 系统生成分享链接并发送

## MQTT数据格式

### 生理数据 (Topic: `ecg/vitalsign`)

```json
{
  "timestamp": "2026-01-07T10:30:00",
  "temperature": 36.5,
  "oxygenSaturation": 98,
  "heartRate": 75,
  "ecgSignal": [0.1, 0.3, 0.5, 0.8, 0.3, -0.2, ...]
}
```

### 报警信息 (Topic: `ecg/alarm`)

```json
{
  "timestamp": "2026-01-07T10:30:00",
  "type": 0,
  "message": "血氧过低",
  "severity": 3
}
```

报警类型枚举：
- 0: 低血氧
- 1: 心率过高
- 2: 心率过低
- 3: 体温异常
- 4: 心电异常

## API接口

### 云端REST API

#### 上传数据
```
POST /api/vitalsign/upload
Header: Authorization: Bearer {token}
Body: {VitalSignData JSON}
```

#### 批量上传
```
POST /api/vitalsign/batch
Body: {
  "deviceId": "xxx",
  "data": [...]
}
```

#### 查询历史
```
GET /api/vitalsign/history?deviceId={id}&startTime={time}&endTime={time}
```

#### 分享数据
```
POST /api/share/create
Body: {
  "deviceId": "xxx",
  "recipientEmail": "xxx@xxx.com",
  "startTime": "...",
  "endTime": "...",
  "validDays": 7
}
```

## 配置文件

应用配置保存在系统默认位置：
- **Windows**: `C:\Users\{User}\AppData\Roaming\ECGApp\ECGMonitor.ini`
- **Android**: `/data/data/com.ecgapp.monitor/shared_prefs/`

配置项：
```ini
[mqtt]
host=localhost
port=1883

[cloud]
server=https://ecg-cloud.com
```

## 数据库Schema

### vital_signs 表
```sql
CREATE TABLE vital_signs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME NOT NULL,
    temperature REAL,
    oxygen_saturation INTEGER,
    heart_rate INTEGER,
    ecg_signal TEXT,
    INDEX idx_timestamp (timestamp)
);
```

### alarms 表
```sql
CREATE TABLE alarms (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME NOT NULL,
    type INTEGER,
    message TEXT,
    severity INTEGER,
    INDEX idx_alarm_timestamp (timestamp)
);
```

## 开发指南

### 添加新的数据类型

1. 在 `VitalSignData.h` 中添加字段
2. 更新 `toJson()` 和 `fromJson()` 方法
3. 修改数据库表结构
4. 更新UI显示组件

### 自定义报警规则

在 `MqttClientManager::parseVitalSignData()` 中添加检测逻辑：

```cpp
if (data.temperature > 38.5) {
    AlarmInfo alarm;
    alarm.type = AlarmInfo::AbnormalTemperature;
    alarm.message = "体温过高: " + QString::number(data.temperature);
    alarm.severity = 3;
    emit alarmReceived(alarm);
}
```

## 常见问题

**Q: MQTT连接失败？**
A: 检查服务器地址、端口、防火墙设置

**Q: 图表不显示？**
A: 确保已安装Qt Charts模块

**Q: Android版本崩溃？**
A: 检查权限配置，确保网络和存储权限已授予

**Q: 数据库错误？**
A: 检查应用数据目录写权限

## 许可证

[待添加许可证信息]

## 贡献指南

欢迎提交Issue和Pull Request

## 联系方式

- 项目主页: [待添加]
- 技术支持: [待添加]

## 版本历史

### v1.0.0 (2026-01-07)
- ✅ 初始版本
- ✅ MQTT通信模块
- ✅ 数据库存储
- ✅ 实时可视化
- ✅ 云同步功能
- ✅ Android支持

---

**开发团队**: ECG App Team  
**最后更新**: 2026-01-07

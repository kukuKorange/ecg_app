# MQTT测试工具使用说明

## 功能

模拟ECG设备通过MQTT发送生理数据和报警信息，用于测试ECG监护系统。

## 安装依赖

```bash
pip install paho-mqtt
```

## 使用方法

### 1. 启动MQTT Broker

推荐使用 Mosquitto:

**Windows:**
```bash
# 下载安装 Mosquitto: https://mosquitto.org/download/
mosquitto -v
```

**Linux/Mac:**
```bash
# 安装
sudo apt-get install mosquitto mosquitto-clients  # Ubuntu/Debian
brew install mosquitto  # macOS

# 启动
mosquitto -v
```

### 2. 运行模拟器

```bash
cd scripts
python mqtt_simulator.py
```

### 3. 启动ECG应用

1. 打开ECG监护系统
2. 配置MQTT服务器: localhost:1883
3. 点击"连接设备"
4. 观察实时数据显示

## 配置

修改 `mqtt_simulator.py` 中的配置：

```python
MQTT_BROKER = "localhost"  # MQTT服务器地址
MQTT_PORT = 1883           # 端口
```

## 输出示例

```
============================================================
ECG MQTT测试工具
============================================================
✓ 已连接到MQTT Broker: localhost:1883

开始发送模拟数据...
按 Ctrl+C 停止

[0000] 发送数据: 体温=36.8°C, 血氧=98%, 心率=72bpm
[0001] 发送数据: 体温=37.1°C, 血氧=99%, 心率=75bpm
     ⚠ 报警: 心率过高 (严重度: 3)
[0002] 发送数据: 体温=36.5°C, 血氧=97%, 心率=68bpm
...
```

## 数据格式

### 生理数据
- 体温: 36.0-37.5°C
- 血氧: 95-100%
- 心率: 60-90 bpm
- ECG信号: 100个采样点/秒

### 报警类型
- 0: 低血氧
- 1: 心率过高
- 2: 心率过低
- 3: 体温异常
- 4: 心电异常

## 故障排除

**Q: 连接失败？**
A: 确保Mosquitto已启动，检查端口是否被占用

**Q: 应用收不到数据？**
A: 检查Topic是否匹配 (`ecg/vitalsign`, `ecg/alarm`)

**Q: 数据格式错误？**
A: 确保JSON格式正确，timestamp为ISO格式

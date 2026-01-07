#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
MQTT测试工具 - 模拟ECG设备发送数据
用于测试ECG监护系统的MQTT接收功能
"""

import paho.mqtt.client as mqtt
from paho.mqtt.client import CallbackAPIVersion
import json
import time
import random
import math
from datetime import datetime

# MQTT配置
MQTT_BROKER = "47.115.148.200"
MQTT_PORT = 1883
MQTT_TOPIC_VITAL = "ecg/vitalsign"
MQTT_TOPIC_ALARM = "ecg/alarm"

def generate_ecg_signal(duration=1.0, sample_rate=100):
    """生成模拟ECG信号（简化的正弦波 + 噪声）"""
    samples = int(duration * sample_rate)
    signal = []
    
    for i in range(samples):
        t = i / sample_rate
        # 基础心跳波形（简化）
        value = 0.5 * math.sin(2 * math.pi * 1.2 * t)  # 1.2Hz ≈ 72 bpm
        
        # 添加R波（QRS复合波）
        if i % 80 < 5:  # 每80个样本点添加一个R波
            value += 1.5
        
        # 添加随机噪声
        value += random.uniform(-0.05, 0.05)
        
        signal.append(round(value, 3))
    
    return signal

def generate_vital_sign_data():
    """生成模拟生理数据"""
    data = {
        "timestamp": datetime.now().isoformat(),
        "temperature": round(random.uniform(36.0, 37.5), 1),
        "oxygenSaturation": random.randint(95, 100),
        "heartRate": random.randint(60, 90),
        "ecgSignal": generate_ecg_signal()
    }
    return data

def generate_alarm(alarm_type=None):
    """生成模拟报警信息"""
    if alarm_type is None:
        alarm_type = random.randint(0, 4)
    
    alarm_messages = [
        "血氧饱和度过低",
        "心率过高",
        "心率过低", 
        "体温异常",
        "心电异常"
    ]
    
    alarm = {
        "timestamp": datetime.now().isoformat(),
        "type": alarm_type,
        "message": alarm_messages[alarm_type],
        "severity": random.randint(1, 5)
    }
    return alarm

def on_connect(client, userdata, flags, reason_code, properties):
    """MQTT连接回调"""
    if reason_code == 0:
        print(f"✓ 已连接到MQTT Broker: {MQTT_BROKER}:{MQTT_PORT}")
    else:
        print(f"✗ 连接失败，返回码: {reason_code}")

def on_publish(client, userdata, mid, reason_code, properties):
    """MQTT发布回调"""
    pass

def main():
    """主函数"""
    print("=" * 60)
    print("ECG MQTT测试工具")
    print("=" * 60)
    
    # 创建MQTT客户端
    client = mqtt.Client(CallbackAPIVersion.VERSION2, "ECG_Simulator")
    client.on_connect = on_connect
    client.on_publish = on_publish
    
    # 连接到Broker
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        client.loop_start()
    except Exception as e:
        print(f"✗ 连接失败: {e}")
        return
    
    time.sleep(1)  # 等待连接建立
    
    print("\n开始发送模拟数据...")
    print("按 Ctrl+C 停止\n")
    
    try:
        counter = 0
        while True:
            # 生成并发送生理数据
            vital_data = generate_vital_sign_data()
            client.publish(MQTT_TOPIC_VITAL, json.dumps(vital_data))
            
            print(f"[{counter:04d}] 发送数据: "
                  f"体温={vital_data['temperature']}°C, "
                  f"血氧={vital_data['oxygenSaturation']}%, "
                  f"心率={vital_data['heartRate']}bpm")
            
            # 随机生成报警（10%概率）
            if random.random() < 0.1:
                alarm_data = generate_alarm()
                client.publish(MQTT_TOPIC_ALARM, json.dumps(alarm_data))
                print(f"     ⚠ 报警: {alarm_data['message']} (严重度: {alarm_data['severity']})")
            
            counter += 1
            time.sleep(1)  # 每秒发送一次
            
    except KeyboardInterrupt:
        print("\n\n停止发送数据...")
    finally:
        client.loop_stop()
        client.disconnect()
        print("已断开连接")

if __name__ == "__main__":
    main()

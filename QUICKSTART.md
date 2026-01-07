# 快速启动指南

## 5分钟上手ECG监护系统

### 步骤1: 环境检查 ✓

确保已安装：
- Qt 6.x (含QtMqtt, QtCharts模块)
- MSVC 2019+ 或 MinGW编译器

检查Qt安装：
```bash
qmake --version
```

### 步骤2: 编译项目 🔨

#### 方法A: 使用Qt Creator（推荐）
```
1. 打开Qt Creator
2. 文件 > 打开文件或项目
3. 选择 ecg_app.pro
4. 配置Kit（MSVC或MinGW）
5. 点击左下角绿色▶按钮运行
```

#### 方法B: 命令行编译
```bash
cd D:\my_proj\ecg_app
qmake ecg_app.pro
nmake          # MSVC
# 或
mingw32-make   # MinGW
```

### 步骤3: 启动MQTT Broker 📡

**安装Mosquitto**（如果没有）:
- Windows: https://mosquitto.org/download/
- 下载并安装后运行：

```bash
mosquitto -v
```

应该看到：
```
1610000000: mosquitto version 2.x running
```

### 步骤4: 运行测试工具 🧪

打开新终端：
```bash
# 安装Python依赖
pip install paho-mqtt

# 运行模拟器
cd D:\my_proj\ecg_app\scripts
python mqtt_simulator.py
```

应该看到：
```
✓ 已连接到MQTT Broker: localhost:1883
开始发送模拟数据...
[0000] 发送数据: 体温=36.8°C, 血氧=98%, 心率=72bpm
```

### 步骤5: 连接应用 🔌

在ECG应用中：
```
1. 点击 文件 > 设置
2. MQTT主机: localhost
3. MQTT端口: 1883
4. 保存设置
5. 点击 文件 > 连接设备
```

状态栏应显示："MQTT已连接"

### 步骤6: 查看数据 📊

现在你应该能看到：
- ✅ 左侧数值面板实时更新
- ✅ 右上方心电图波形滚动
- ✅ 右下方趋势图显示
- ✅ 数据自动保存到数据库

### 功能验证清单

- [ ] MQTT连接成功
- [ ] 实时数据显示
- [ ] 心电图波形正常
- [ ] 趋势图绘制
- [ ] 报警提示（偶尔出现）
- [ ] 历史查询功能
- [ ] 数据导出CSV

## Android版本快速构建

### 前提条件
- Qt for Android已安装
- Android SDK/NDK已配置

### 构建步骤
```
1. Qt Creator > 项目模式
2. 添加 Android Kit
3. 选择 Android Kit
4. 构建 > 部署
5. APK生成位置会显示在"编译输出"窗口
```

## 常见问题快速解决

### 问题：编译错误 "找不到QtMqtt"
**解决**: 
```bash
# 安装QtMqtt模块
# Qt Maintenance Tool > Add or remove components > Qt > Qt x.x.x > Additional Libraries > Qt MQTT
```

### 问题：MQTT连接失败
**检查**:
1. Mosquitto是否运行？
2. 端口1883是否被占用？
3. 防火墙是否阻止？

### 问题：图表不显示
**解决**: 确保已安装Qt Charts模块

### 问题：Python模拟器报错
**解决**: 
```bash
pip install --upgrade paho-mqtt
```

## 下一步

✅ **基础测试通过后**，可以：

1. **配置真实MQTT设备**
   - 修改mqtt_simulator.py为你的设备协议
   - 或连接硬件ECG设备

2. **配置云服务器**
   - 修改CloudSyncManager中的服务器URL
   - 实现后端API接口

3. **自定义UI**
   - 修改ecg_app.ui设计界面
   - 调整图表显示样式

4. **添加新功能**
   - 参考README.md开发指南
   - 扩展数据类型和报警规则

## 技术支持

遇到问题？查看：
- 📖 [README.md](README.md) - 完整文档
- 📱 [ANDROID_BUILD.md](ANDROID_BUILD.md) - Android构建
- 🧪 [scripts/MQTT_SIMULATOR.md](scripts/MQTT_SIMULATOR.md) - 测试工具

---

**祝你使用愉快！** 🎉

# 🚀 立即开始使用

## 最快方式：使用Qt Creator

### 1️⃣ 打开项目
```
1. 启动 Qt Creator
2. 文件 → 打开文件或项目
3. 选择: D:\my_proj\ecg_app\ecg_app.pro
4. 配置Kit（选择MSVC或MinGW）
```

### 2️⃣ 点击运行
```
点击左下角绿色 ▶ 按钮
或按 F5 / Ctrl+R
```

**就这么简单！** 🎉

---

## 完整测试流程

### 准备工作

#### A. 安装Mosquitto（MQTT服务器）
1. 下载：https://mosquitto.org/download/
2. 安装后运行：
   ```cmd
   mosquitto -v
   ```

#### B. 安装Python依赖（数据模拟器）
```cmd
pip install paho-mqtt
```

### 三步启动

#### 方式1: 自动启动（推荐）
双击运行：
```
scripts\run_test_env.bat
```
会自动启动所有组件！

#### 方式2: 手动启动
**终端1 - MQTT Broker:**
```cmd
mosquitto -v
```

**终端2 - 数据模拟器:**
```cmd
cd D:\my_proj\ecg_app\scripts
python mqtt_simulator.py
```

**终端3 - ECG应用:**
- Qt Creator中按F5运行
- 或运行编译好的exe

### 使用应用

1. **连接设备**
   - 文件 → 设置
   - MQTT服务器: `localhost`
   - 端口: `1883`
   - 保存设置
   - 文件 → 连接设备

2. **查看数据**
   - 实时监护标签：查看当前数值和波形
   - 历史查询标签：查看历史趋势
   - 状态栏会显示连接状态

3. **测试功能**
   - ✅ 实时数据显示
   - ✅ 心电图波形
   - ✅ 趋势图
   - ✅ 报警提示（随机出现）
   - ✅ 数据导出CSV

---

## 常见问题

### ❓ Qt Creator找不到项目？
**解决**: 确保打开的是 `ecg_app.pro` 文件

### ❓ 编译错误：找不到QtMqtt？
**解决**: 
1. 工具 → Qt Maintenance Tool
2. Add or remove components
3. Qt 6.x → Additional Libraries → 勾选Qt MQTT
4. 安装

### ❓ 找不到Charts模块？
**解决**: 同上，勾选Qt Charts模块

### ❓ 应用连接不上MQTT？
**检查**:
- Mosquitto是否运行？（看终端有输出）
- 端口1883是否被占用？
- 防火墙是否允许？

### ❓ 看不到数据？
**检查**:
- 数据模拟器是否运行？
- 应用是否已连接？（状态栏显示"MQTT已连接"）
- Topic是否匹配？

---

## 编译说明

### 使用命令行编译

**打开Qt命令提示符**（开始菜单搜索"Qt 6"）

```cmd
cd /d D:\my_proj\ecg_app
scripts\build_windows.bat
```

**或手动编译:**
```cmd
cd /d D:\my_proj\ecg_app
mkdir build-debug
cd build-debug
qmake ..\ecg_app.pro "CONFIG+=debug"
nmake           # MSVC
# 或
mingw32-make    # MinGW
```

---

## 详细文档

- 📖 [完整项目文档](README.md)
- 🪟 [Windows编译详解](BUILD_WINDOWS.md)
- 📱 [Android编译指南](ANDROID_BUILD.md)
- 🧪 [测试工具说明](scripts/MQTT_SIMULATOR.md)

---

## 需要帮助？

### 文档位置
```
D:\my_proj\ecg_app\
├── START_HERE.md           ← 你在这里
├── BUILD_WINDOWS.md        ← Windows编译详解
├── README.md               ← 完整文档
└── QUICKSTART.md          ← 5分钟快速入门
```

### 推荐阅读顺序
1. ✅ START_HERE.md (当前文件)
2. BUILD_WINDOWS.md (如遇编译问题)
3. README.md (了解所有功能)

---

**祝你使用愉快！** 🎊

有问题？检查 [BUILD_WINDOWS.md](BUILD_WINDOWS.md) 中的故障排查章节。

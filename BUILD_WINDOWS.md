# Windows平台编译运行指南

## 方法一：使用Qt Creator（推荐）⭐

这是最简单的方法，Qt Creator会自动处理所有配置。

### 步骤：

1. **打开Qt Creator**
   - 启动Qt Creator应用

2. **打开项目**
   - 文件 → 打开文件或项目
   - 浏览到：`D:\my_proj\ecg_app\ecg_app.pro`
   - 点击"打开"

3. **配置Kit**
   - Qt Creator会显示"Configure Project"页面
   - 选择一个Kit（推荐Desktop Qt 6.x.x MSVC2019 64bit）
   - 点击"Configure Project"

4. **编译运行**
   - 点击左下角的"▶"（运行）按钮
   - 或按快捷键：Ctrl+R
   - 首次编译可能需要几分钟

5. **如果出现QtMqtt错误**
   - 工具 → 选项 → Kits → Qt Versions
   - 确认你的Qt版本包含QtMqtt模块
   - 如果没有，使用Qt Maintenance Tool安装：
     * 打开Qt Maintenance Tool
     * Add or remove components
     * Qt → Qt 6.x.x → Additional Libraries → Qt MQTT

## 方法二：使用命令行

### 前提条件：
- 已安装Qt 6.x
- 已安装Visual Studio 2019/2022（需要MSVC编译器）

### 步骤：

1. **打开"Qt 6.x for Desktop (MSVC 2019 64-bit)"命令提示符**
   - 在开始菜单搜索："Qt 6"
   - 找到并打开对应的命令提示符

2. **进入项目目录**
   ```cmd
   cd /d D:\my_proj\ecg_app
   ```

3. **生成Makefile**
   ```cmd
   qmake ecg_app.pro -spec win32-msvc
   ```

4. **编译项目**
   ```cmd
   nmake
   ```
   或者编译Release版本：
   ```cmd
   qmake ecg_app.pro -spec win32-msvc "CONFIG+=release"
   nmake release
   ```

5. **运行程序**
   ```cmd
   # Debug版本
   debug\ecg_app.exe
   
   # Release版本
   release\ecg_app.exe
   ```

## 方法三：使用提供的批处理脚本

运行现有的编译脚本：

```cmd
# Debug版本
scripts\build_debug.bat debug

# Release版本
scripts\build_release.bat release
```

## 常见问题排查

### 问题1：找不到qmake
**原因**: Qt没有添加到系统PATH

**解决方案**:
- 使用Qt自带的命令提示符（方法二）
- 或添加Qt bin目录到系统PATH：
  * 通常在: `C:\Qt\6.x.x\msvc2019_64\bin`
  * 添加到系统环境变量PATH

### 问题2：QtMqtt模块未找到
**错误信息**: `Unknown module(s) in QT: mqtt`

**解决方案**:
1. 打开Qt Maintenance Tool
2. Add or remove components
3. 展开 Qt → Qt 6.x.x → Additional Libraries
4. 勾选"Qt MQTT"
5. 安装

### 问题3：缺少MSVC编译器
**错误信息**: `Cannot find nmake`

**解决方案**:
- 安装Visual Studio 2019或2022
- 确保安装了"Desktop development with C++"工作负载
- 或使用MinGW版本的Qt

### 问题4：Charts模块错误
**错误信息**: `Unknown module(s) in QT: charts`

**解决方案**:
与QtMqtt类似，使用Qt Maintenance Tool安装Qt Charts模块

## 编译成功标志

编译成功后，你会在项目目录看到：
```
ecg_app/
├── debug/
│   └── ecg_app.exe          # Debug版本可执行文件
├── release/
│   └── ecg_app.exe          # Release版本可执行文件
└── *.o 或 *.obj 文件       # 编译的对象文件
```

## 下一步：测试运行

编译成功后，按照以下步骤测试：

1. **启动MQTT Broker**
   ```cmd
   mosquitto -v
   ```
   如果没有安装，下载：https://mosquitto.org/download/

2. **运行数据模拟器**（新终端）
   ```cmd
   cd D:\my_proj\ecg_app\scripts
   pip install paho-mqtt
   python mqtt_simulator.py
   ```

3. **启动ECG应用**
   - 运行编译好的ecg_app.exe
   - 或在Qt Creator中按F5运行

4. **连接设备**
   - 文件 → 设置 → 配置MQTT（localhost:1883）
   - 文件 → 连接设备
   - 查看实时数据显示

## 性能提示

### Debug vs Release
- **Debug**: 包含调试信息，方便开发调试
- **Release**: 优化编译，运行速度更快，文件更小

### 首次编译
- 首次编译会较慢（5-10分钟）
- 后续增量编译会快很多（几秒到1分钟）

### 清理编译
如果遇到奇怪的编译错误，可以清理后重新编译：
```cmd
nmake clean
qmake ecg_app.pro
nmake
```

或在Qt Creator中：
- 构建 → 清理项目
- 构建 → 重新构建项目

---

**推荐**: 初次使用建议用Qt Creator（方法一），最简单直接！

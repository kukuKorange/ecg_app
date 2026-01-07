@echo off
REM ============================================
REM 一键启动 ECG 监护系统测试环境
REM ============================================

echo ========================================
echo ECG 监护系统 - 测试环境启动器
echo ========================================
echo.

REM 进入项目根目录
cd /d %~dp0..

echo [1/4] 检查MQTT Broker (Mosquitto)...
where mosquitto >nul 2>nul
if %errorlevel% neq 0 (
    echo [!] 未找到Mosquitto
    echo.
    echo 请先安装Mosquitto MQTT Broker:
    echo   下载地址: https://mosquitto.org/download/
    echo   或者使用在线MQTT服务器
    echo.
    echo 按任意键跳过MQTT检查并继续...
    pause >nul
    goto :skip_mqtt
)

echo [✓] 找到Mosquitto
echo [启动] 正在启动MQTT Broker...
start "MQTT Broker" mosquitto -v
timeout /t 2 >nul
echo [✓] MQTT Broker已启动
echo.

:skip_mqtt

echo [2/4] 检查Python和依赖...
where python >nul 2>nul
if %errorlevel% neq 0 (
    echo [!] 未找到Python
    echo 跳过数据模拟器
    goto :skip_simulator
)

echo [✓] 找到Python
python -c "import paho.mqtt.client" 2>nul
if %errorlevel% neq 0 (
    echo [安装] 正在安装paho-mqtt...
    pip install paho-mqtt
)

echo [启动] 正在启动数据模拟器...
start "ECG Data Simulator" python scripts\mqtt_simulator.py
timeout /t 2 >nul
echo [✓] 数据模拟器已启动
echo.

:skip_simulator

echo [3/4] 检查应用程序...
if exist build-debug\debug\ecg_app.exe (
    set APP_PATH=build-debug\debug\ecg_app.exe
    goto :found_app
)
if exist build-debug\ecg_app.exe (
    set APP_PATH=build-debug\ecg_app.exe
    goto :found_app
)
if exist debug\ecg_app.exe (
    set APP_PATH=debug\ecg_app.exe
    goto :found_app
)
if exist release\ecg_app.exe (
    set APP_PATH=release\ecg_app.exe
    goto :found_app
)

echo [!] 未找到编译好的应用程序
echo.
echo 请先编译项目:
echo   1. 使用Qt Creator打开 ecg_app.pro
echo   2. 点击运行按钮编译
echo   或运行: scripts\build_windows.bat
echo.
pause
exit /b 1

:found_app
echo [✓] 找到应用程序: %APP_PATH%
echo.

echo [4/4] 启动ECG应用程序...
start "ECG Monitor" %APP_PATH%
timeout /t 1 >nul
echo [✓] 应用程序已启动
echo.

echo ========================================
echo 所有组件已启动！
echo ========================================
echo.
echo 已启动的服务:
echo   [✓] MQTT Broker (mosquitto)
echo   [✓] 数据模拟器 (mqtt_simulator.py)
echo   [✓] ECG监护应用 (ecg_app.exe)
echo.
echo 接下来的步骤:
echo   1. 在应用中: 文件 → 设置
echo   2. 配置MQTT: localhost:1883
echo   3. 点击: 文件 → 连接设备
echo   4. 查看实时数据显示
echo.
echo 按任意键关闭此窗口...
pause >nul

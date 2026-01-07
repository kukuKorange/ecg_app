@echo off
chcp 65001 >nul 2>&1
echo.
echo ========================================
echo    ECG 监护系统 - 启动菜单
echo ========================================
echo.
echo 请选择操作:
echo.
echo  [1] 运行 ECG 应用程序
echo  [2] 使用 Qt Creator 打开项目
echo  [3] 编译项目 (MinGW)
echo  [4] 启动测试环境 (MQTT + 模拟器 + 应用)
echo  [5] 仅启动 MQTT Broker
echo  [6] 仅启动数据模拟器
echo  [7] 查看项目文档
echo  [0] 退出
echo.
choice /C 12345670 /N /M "请输入选项 (1-7 或 0): "

if errorlevel 8 goto :exit
if errorlevel 7 goto :docs
if errorlevel 6 goto :simulator
if errorlevel 5 goto :mqtt
if errorlevel 4 goto :test_env
if errorlevel 3 goto :compile
if errorlevel 2 goto :qtcreator
if errorlevel 1 goto :run_app

:run_app
echo.
echo 正在启动 ECG 应用程序...
if exist "%~dp0build\debug\ecg_app.exe" (
    start "" "%~dp0build\debug\ecg_app.exe"
    echo [OK] ECG 应用程序已启动
) else (
    echo [!] 未找到可执行文件
    echo     请先编译项目 (选项 3)
)
timeout /t 2 >nul
goto :menu

:qtcreator
echo.
echo 正在查找 Qt Creator...
where qtcreator >nul 2>nul
if %errorlevel% neq 0 (
    echo [!] 未找到 qtcreator 命令
    echo.
    echo 请手动打开Qt Creator，然后:
    echo   1. 文件 → 打开文件或项目
    echo   2. 选择: %~dp0ecg_app.pro
    echo.
    pause
    goto :menu
)
start qtcreator "%~dp0ecg_app.pro"
echo [OK] Qt Creator 已启动
timeout /t 2 >nul
goto :menu

:compile
echo.
echo 正在编译项目...
echo.
set PATH=D:\qt-everywhere-src-6.9.3\qt\6.10.1\mingw_64\bin;D:\qt-everywhere-src-6.9.3\qt\Tools\mingw1310_64\bin;%PATH%
cd /d "%~dp0build"
echo [1/2] 编译中...
mingw32-make -j4
if %errorlevel% neq 0 (
    echo [!] 编译失败
    pause
    cd /d "%~dp0"
    goto :menu
)
echo.
echo [2/2] 部署 Qt 依赖库...
cd /d "%~dp0build\debug"
windeployqt.exe ecg_app.exe >nul 2>&1
echo.
echo [OK] 编译完成！
echo     可执行文件: %~dp0build\debug\ecg_app.exe
cd /d "%~dp0"
pause
goto :menu

:test_env
echo.
call "%~dp0scripts\run_test_env.bat"
goto :menu

:mqtt
echo.
echo 正在启动 MQTT Broker...
where mosquitto >nul 2>nul
if %errorlevel% neq 0 (
    echo [!] 未找到 Mosquitto
    echo 请先安装: https://mosquitto.org/download/
    pause
    goto :menu
)
start "MQTT Broker - Mosquitto" mosquitto -v
echo [OK] MQTT Broker 已在新窗口启动
timeout /t 2 >nul
goto :menu

:simulator
echo.
echo 正在启动数据模拟器...
where python >nul 2>nul
if %errorlevel% neq 0 (
    echo [!] 未找到 Python
    pause
    goto :menu
)
cd /d "%~dp0scripts"
start "ECG 数据模拟器" python mqtt_simulator.py
echo [OK] 数据模拟器已在新窗口启动
timeout /t 2 >nul
cd /d "%~dp0"
goto :menu

:docs
echo.
echo 可用文档:
echo.
echo  [1] START_HERE.md      - 快速开始
echo  [2] BUILD_WINDOWS.md   - Windows编译
echo  [3] README.md          - 完整文档
echo  [4] QUICKSTART.md      - 5分钟入门
echo  [0] 返回主菜单
echo.
choice /C 12340 /N /M "请选择 (1-4 或 0): "
if errorlevel 5 goto :menu
if errorlevel 4 start "" "QUICKSTART.md"
if errorlevel 3 start "" "README.md"
if errorlevel 2 start "" "BUILD_WINDOWS.md"
if errorlevel 1 start "" "START_HERE.md"
goto :menu

:menu
echo.
echo ========================================
echo.
choice /C YN /M "是否返回主菜单"
if errorlevel 2 goto :exit
cls
goto :start

:exit
echo.
echo 再见！
timeout /t 1 >nul
exit /b 0

:start
goto :eof

@echo off
echo 正在启动 ECG 监护系统...
cd /d "%~dp0build\debug"
if exist ecg_app.exe (
    start "" ecg_app.exe
) else (
    echo [错误] 未找到 ecg_app.exe
    echo 请先运行 ECG_LAUNCHER.bat 并选择编译选项
    pause
)


@echo off
REM ============================================
REM ECG应用 Windows编译脚本
REM 使用Qt Creator的环境来编译项目
REM ============================================

echo ========================================
echo 正在编译 ECG 监护系统 (Debug版本)
echo ========================================
echo.

REM 检查是否在Qt环境中
where qmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [错误] 未找到qmake命令！
    echo.
    echo 请执行以下步骤之一：
    echo.
    echo 方法1: 使用Qt提供的命令提示符
    echo   - 打开 "Qt 6.x for Desktop (MSVC/MinGW)" 命令提示符
    echo   - 进入项目目录: cd /d %~dp0..
    echo   - 重新运行此脚本
    echo.
    echo 方法2: 使用Qt Creator
    echo   - 打开Qt Creator
    echo   - 打开项目文件: ecg_app.pro
    echo   - 点击运行按钮
    echo.
    echo 方法3: 手动设置PATH
    echo   - 将Qt的bin目录添加到PATH
    echo   - 例如: set PATH=C:\Qt\6.x.x\msvc2019_64\bin;%%PATH%%
    echo.
    pause
    exit /b 1
)

echo [✓] 找到qmake: 
qmake -v
echo.

REM 进入项目根目录
cd /d %~dp0..

REM 创建构建目录
if not exist build-debug mkdir build-debug
cd build-debug

echo [步骤 1/3] 生成Makefile...
qmake ..\ecg_app.pro "CONFIG+=debug" "CONFIG+=qml_debug"
if %errorlevel% neq 0 (
    echo [错误] qmake执行失败！
    echo 可能原因：
    echo   - 缺少Qt模块 (mqtt, charts, sql)
    echo   - .pro文件语法错误
    pause
    exit /b 1
)
echo [✓] Makefile生成成功
echo.

echo [步骤 2/3] 编译项目...
REM 检测编译器类型
if exist Makefile.Debug (
    REM MSVC编译器
    nmake
) else (
    REM MinGW编译器
    mingw32-make
)

if %errorlevel% neq 0 (
    echo [错误] 编译失败！
    echo 请检查编译错误信息
    pause
    exit /b 1
)
echo [✓] 编译成功
echo.

echo [步骤 3/3] 部署Qt依赖...
if exist debug\ecg_app.exe (
    cd debug
    windeployqt ecg_app.exe --no-translations
    cd ..
    echo [✓] 依赖部署完成
) else if exist ecg_app.exe (
    windeployqt ecg_app.exe --no-translations
    echo [✓] 依赖部署完成
) else (
    echo [警告] 未找到可执行文件
)
echo.

echo ========================================
echo 编译完成！
echo ========================================
echo.
echo 可执行文件位置：
if exist debug\ecg_app.exe (
    echo   %cd%\debug\ecg_app.exe
) else if exist ecg_app.exe (
    echo   %cd%\ecg_app.exe
) else (
    echo   [未找到]
)
echo.
echo 运行程序:
if exist debug\ecg_app.exe (
    echo   cd build-debug\debug
    echo   ecg_app.exe
) else if exist ecg_app.exe (
    echo   cd build-debug
    echo   ecg_app.exe
)
echo.

choice /C YN /M "是否现在运行程序"
if %errorlevel%==1 (
    if exist debug\ecg_app.exe (
        start debug\ecg_app.exe
    ) else if exist ecg_app.exe (
        start ecg_app.exe
    )
)

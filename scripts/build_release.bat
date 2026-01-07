@echo off
title qmake and nmake build prompt
set MINGW_PATH=D:\qt-everywhere-src-6.9.3\qt\Tools\mingw0_64
set QT_DIR=D:\qt-everywhere-src-6.9.3\qt\6.10.1\mingw_64
set BUILD_DIR=%cd%\build
set PRO_DIR=%cd%
set PATH=%MINGW_PATH%\bin;%QT_DIR%\bin;%PATH%


if not exist %BUILD_DIR% (
    md %BUILD_DIR%
)

cd build
qmake.exe %PRO_DIR%\ecg_app.pro -spec win32-g++ "CONFIG+=release"
if exist %BUILD_DIR%\release\ecg_app.exe del %BUILD_DIR%\release\ecg_app.exe
@REM D:\qt-everywhere-src-6.9.3\qt\Tools\QtCreator\bin\jom.exe -j4
%MINGW_PATH%\bin\mingw32-make -f Makefile.Release
cd release
if not exist %BUILD_DIR%\release\Qt5Core.dll (
    windeployqt ecg_app.exe
)
# QtMqtt需要单独安装，暂时禁用
# QT += core gui widgets mqtt sql charts network
QT += core gui widgets sql charts network

# 定义宏禁用MQTT功能
DEFINES += NO_MQTT_SUPPORT

CONFIG += c++17

win32-msvc*:QMAKE_CXXFLAGS += /utf-8
# QMAKE_LFLAGS += "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""

SOURCES += \
    $$files($$PWD/src/*.cpp)

HEADERS += \
    $$files($$PWD/src/*.h)

FORMS += $$PWD/src/ecg_app.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Android配置
include(android_config.pri)
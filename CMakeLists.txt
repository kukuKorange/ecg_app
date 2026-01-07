cmake_minimum_required(VERSION 3.16)
project(ecg_app VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找 Qt 包 (包含 MQTT 模块)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Charts Network Mqtt)

# 收集源文件
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")
file(GLOB FORMS "src/*.ui")

# 创建可执行文件 (临时去掉 WIN32 以便在控制台看到输出)
add_executable(ecg_app
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
)

# 链接 Qt 库
target_link_libraries(ecg_app PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
    Qt6::Network
    Qt6::Mqtt
)

# 设置包含目录
target_include_directories(ecg_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)


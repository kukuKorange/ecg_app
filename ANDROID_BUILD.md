# Android 部署配置

## 项目特性
- **框架**: Qt 6 for Android
- **语言**: C++17
- **模块**: MQTT, SQL, Charts, Network
- **目标**: Android APK

## 环境准备

### 必需工具
1. **Qt for Android**
   - 下载 Qt Online Installer
   - 安装组件：
     - Qt 6.x for Android
     - Android SDK (API 31+)
     - Android NDK (r23+)
     - OpenJDK 11+

2. **Android Studio** (推荐) 或
3. **Qt Creator** 配置 Android Kit

## 配置步骤

### 1. 在Qt Creator中配置Android Kit

打开 **工具 > 选项 > 设备 > Android**，设置：
- JDK路径
- Android SDK路径
- Android NDK路径

### 2. 修改.pro文件Android配置

已在 `ecg_app.pro` 中添加：
```qmake
android {
    QT += androidextras
    
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    
    ANDROID_MIN_SDK_VERSION = 23
    ANDROID_TARGET_SDK_VERSION = 33
    
    # 权限配置将在 AndroidManifest.xml 中设置
}
```

### 3. 创建 Android 清单文件

创建 `android/AndroidManifest.xml`：
```xml
<?xml version="1.0"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
          package="com.ecgapp.monitor"
          android:versionCode="1"
          android:versionName="1.0">
    
    <uses-permission android:name="android.permission.INTERNET"/>
    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE"/>
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
    <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
    
    <application android:label="ECG监护"
                 android:icon="@drawable/icon">
        <!-- Qt配置将自动生成 -->
    </application>
</manifest>
```

## 构建APK

### 方法1: Qt Creator
1. 选择 Android Kit
2. 点击 **构建 > 构建项目**
3. 点击 **构建 > 部署**
4. APK位置: `build-ecg_app-Android_Qt_6_x_Clang_arm64_v8a-Release/android-build/build/outputs/apk/release/`

### 方法2: 命令行
```bash
# 设置环境变量
export ANDROID_SDK_ROOT=/path/to/android/sdk
export ANDROID_NDK_ROOT=/path/to/android/ndk
export JAVA_HOME=/path/to/jdk

# 配置构建
qmake -spec android-clang CONFIG+=release

# 编译
make -j4

# 生成APK
androiddeployqt --input android-ecg_app-deployment-settings.json --output android-build --android-platform android-33 --jdk $JAVA_HOME --gradle
```

## 签名APK (Release版本)

### 生成密钥库
```bash
keytool -genkey -v -keystore ecg-release.keystore -alias ecg-key -keyalg RSA -keysize 2048 -validity 10000
```

### 签名APK
```bash
jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore ecg-release.keystore android-build/build/outputs/apk/release/app-release-unsigned.apk ecg-key

zipalign -v 4 app-release-unsigned.apk ECG-Monitor-v1.0.apk
```

## 测试

### 模拟器测试
```bash
# 列出可用模拟器
emulator -list-avds

# 启动模拟器
emulator -avd Pixel_5_API_33

# 安装APK
adb install ECG-Monitor-v1.0.apk
```

### 真机测试
1. 启用开发者选项和USB调试
2. 连接设备
3. Qt Creator会自动检测设备
4. 点击运行

## 功能验证清单

- [ ] MQTT连接和数据接收
- [ ] 数据库读写（SQLite）
- [ ] 图表显示（实时波形和趋势）
- [ ] 网络同步（云端上传/下载）
- [ ] 数据导出（CSV）
- [ ] 报警通知
- [ ] 触摸交互
- [ ] 横竖屏切换

## 优化建议

### 性能优化
1. 启用ProGuard混淆（可选）
2. 使用发布配置：`CONFIG+=release`
3. 启用优化：`QMAKE_CXXFLAGS += -O3`

### APK瘦身
```qmake
# 在.pro文件中添加
android {
    ANDROID_ABIS = arm64-v8a  # 只构建arm64，减小APK大小
}
```

### 图标和启动画面
- 准备不同分辨率的图标: 48x48, 72x72, 96x96, 144x144, 192x192
- 放置在 `android/res/drawable-*/icon.png`

## 发布到应用商店

### Google Play
1. 创建开发者账号
2. 准备应用描述和截图
3. 上传签名的APK
4. 设置定价和分发

### 其他市场
- 小米应用商店
- 华为应用市场
- 腾讯应用宝
- 360手机助手

## 故障排除

### 常见问题

**问题1**: 找不到Qt模块
- 确保Qt for Android已正确安装
- 检查Kit配置

**问题2**: 网络权限被拒绝
- 检查 AndroidManifest.xml 权限配置
- Android 6.0+ 需要运行时权限

**问题3**: APK安装失败
- 检查签名
- 确保目标API版本匹配

## 技术支持

- Qt官方文档: https://doc.qt.io/qt-6/android.html
- Qt论坛: https://forum.qt.io/
- 项目GitHub: [待添加]

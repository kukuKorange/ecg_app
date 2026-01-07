android {
    QT += androidextras
    
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    
    ANDROID_MIN_SDK_VERSION = 23
    ANDROID_TARGET_SDK_VERSION = 33
    
    # 仅构建arm64-v8a以减小APK大小，可根据需要添加其他架构
    # ANDROID_ABIS = arm64-v8a armeabi-v7a
    ANDROID_ABIS = arm64-v8a
}

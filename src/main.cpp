#include "ecg_app.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>

#ifdef _WIN32
#pragma comment(lib, "user32.lib")
#endif

// 自定义日志处理器
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QFile logFile(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/ecg_app.log");
    if (!logFile.isOpen()) {
        logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    
    QTextStream out(&logFile);
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    
    switch (type) {
    case QtDebugMsg:    out << timestamp << " [DEBUG] " << msg << "\n"; break;
    case QtInfoMsg:     out << timestamp << " [INFO] " << msg << "\n"; break;
    case QtWarningMsg:  out << timestamp << " [WARN] " << msg << "\n"; break;
    case QtCriticalMsg: out << timestamp << " [CRITICAL] " << msg << "\n"; break;
    case QtFatalMsg:    out << timestamp << " [FATAL] " << msg << "\n"; break;
    }
    out.flush();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);
    qDebug() << "=== ECG App Starting ===";
    
    QApplication a(argc, argv);
    a.setApplicationName("ECG监护系统");
    a.setApplicationVersion("1.0");
    
    qDebug() << "Creating main window...";
    ecg_app w;
    
    qDebug() << "Showing window...";
    w.show();
    
    qDebug() << "Entering event loop...";
    return a.exec();
}
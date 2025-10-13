#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QSettings>
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 加载QSS样式
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug("Open qss success.");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }
    else{
        qDebug("Open qss failed.");
    }



    QString fileName = "config.ini";
    QString app_path = QCoreApplication::applicationDirPath();
    // 尝试两个可能的路径
    QStringList possiblePaths;
    possiblePaths << app_path + "/" + fileName;         // 同级目录
    possiblePaths << app_path + "/../" + fileName;      // 上一级目录
    QString config_path;
    for(const QString &path : possiblePaths) {
        if(QFile::exists(path)) {
            config_path = path;
            break;
        }
    }
    if(config_path.isEmpty()) {
        qFatal("Config file not found in any expected location!");
    }
    // QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + fileName); // ?
    QSettings settings(config_path,QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://"+gate_host+":"+gate_port;

    qDebug()<<"the path"<<gate_url_prefix;

    MainWindow w;
    w.show();
    return a.exec();
}

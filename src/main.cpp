/**
 * @file main.cpp
 * @author Nikolay Yevik 
 * @brief Entry point for the TurboRPM Package Manager Prototype application.
 * @version 0.0.1
 * @date 2025-12-6
 */
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QCoreApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationDisplayName("TurboRPM Package Manager Prototype");
    const QIcon appIcon(":/src/icons/yumex.png");
    QApplication::setWindowIcon(appIcon);
    QApplication::setQuitOnLastWindowClosed(true);
    QCoreApplication::setApplicationName("TurboRPM");
    QCoreApplication::setOrganizationName("YEVIK"); 
    QCoreApplication::setApplicationVersion("0.0.1");
    QCoreApplication::setOrganizationDomain("yevik.com");
    QString appName = QCoreApplication::applicationName();
    QString orgName = QCoreApplication::organizationName();
    QString appVersion = QCoreApplication::applicationVersion();

    #ifdef QT_DEBUG
    QDebug dbg(QtDebugMsg);//on stack
    dbg << "Main Icon exists? " << QFile(":/src/icons/yumex.png").exists() << Qt::endl;
    dbg << "Main Icon isNull? " << appIcon.isNull() << Qt::endl;
    dbg << "Application Name: " << appName << Qt::endl;
    dbg << "Organization Name: " << orgName << Qt::endl;
    dbg << "TurboRPM Application Version: " << appVersion << Qt::endl;
    dbg << "Qt Version: " << QT_VERSION_STR << Qt::endl;
    dbg << "C++ Standard Version: " << __cplusplus << Qt::endl;
    dbg << "Platform: " << QSysInfo::prettyProductName() << Qt::endl;
    dbg << "Build CPU Architecture: " << QSysInfo::buildCpuArchitecture() << Qt::endl;
    dbg << "Current CPU Architecture: " << QSysInfo::currentCpuArchitecture() << Qt::endl;
    dbg << "Kernel Type: " << QSysInfo::kernelType() << Qt::endl;
    dbg << "Kernel Version: " << QSysInfo::kernelVersion() << Qt::endl;
    dbg << "Word Size (bits): " << QSysInfo::WordSize << Qt::endl;
    dbg << "Byte Order: "
         << (QSysInfo::ByteOrder == QSysInfo::BigEndian ? "Big Endian" : "Little Endian")
         << Qt::endl;
    dbg << "Machine Host Name: " << QSysInfo::machineHostName() << Qt::endl;
    dbg << "Machine Unique ID (hex): " << QSysInfo::machineUniqueId().toHex() << Qt::endl;
    dbg << "Boot Unique ID (hex): " << QSysInfo::bootUniqueId().toHex() << Qt::endl;
    dbg << "Product Version: " << QSysInfo::productVersion() << Qt::endl;

    #endif
    
    MainWindow w;
    w.setAcceptDrops(true); //obscured by central widget and QTableView, JIC.
    w.show();

    return app.exec();
}

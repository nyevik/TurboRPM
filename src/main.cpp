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
#include <QSysInfo>
#include <QLoggingCategory>
#include "mainwindow.h"

#ifdef QT_DEBUG
static inline QDebug DBG()
{
    return qDebug().noquote().nospace();
}
#endif
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
    DBG() << "=== TurboRPM Package Manager Prototype Debug Info ===\n";
    DBG() << "Main Icon exists? " << QFile(":/src/icons/yumex.png").exists();
    DBG() << "Main Icon isNull? " << appIcon.isNull();
    DBG() << "Application Name: " << appName;
    DBG() << "Organization Name: " << orgName;
    DBG() << "TurboRPM Application Version: " << appVersion;
    DBG() << "Qt Version: " << QT_VERSION_STR;
    DBG() << "C++ Standard Version: " << __cplusplus;
    DBG() << "Platform: " << QSysInfo::prettyProductName();
    DBG() << "Build CPU Architecture: " << QSysInfo::buildCpuArchitecture();
    DBG() << "Current CPU Architecture: " << QSysInfo::currentCpuArchitecture();
    DBG() << "Kernel Type: " << QSysInfo::kernelType();
    DBG() << "Kernel Version: " << QSysInfo::kernelVersion();
    DBG() << "Word Size (bits): " << QSysInfo::WordSize;
    DBG() << "Byte Order: "
         << (QSysInfo::ByteOrder == QSysInfo::BigEndian ? "Big Endian" : "Little Endian")
         << "\n";
    DBG() << "Machine Host Name: " << QSysInfo::machineHostName() << "\n";
    DBG() << "Machine Unique ID (hex): " << QSysInfo::machineUniqueId().toHex();
    DBG() << "Boot Unique ID (hex): " << QSysInfo::bootUniqueId().toHex();
    DBG() << "Product Version: " << QSysInfo::productVersion();
    DBG() << "Product Type: " << QSysInfo::productType();

    #endif
    
    MainWindow w;
    w.setAcceptDrops(true); //obscured by central widget and QTableView, JIC.
    w.show();

    return app.exec();
}

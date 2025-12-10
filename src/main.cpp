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
    qDebug() << "Icon exists?"
         << QFile(":/src/icons/yumex.png").exists();
    

    qDebug() << "Icon isNull?"
         << appIcon.isNull();
    qDebug() << "Application Name:" << appName;
    qDebug() << "Organization Name:" << orgName;
    qDebug() << "Application Version:" << appVersion;
    #endif
    
    MainWindow w;
    w.setAcceptDrops(true); //obscured by central widget and QTableView, JIC.
    w.show();

    return app.exec();
}

/**
 * @file qt_thread_test.cpp
 * @author Nikolay Yevik
 * @brief Test application to compare QThread and std::jthread thread IDs.
 * @version 0.0.1
 * @date 2025-12-6
 * @copyright Copyright (c) 2025 Nikolay Yevik
 */

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>

#include "qtworker.h"

#include <thread>
#include <chrono>
#include <pthread.h>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    auto mainQtId = reinterpret_cast<quintptr>(QThread::currentThreadId());
    auto mainPtId = pthread_self();

    qDebug().nospace()
        << "Main thread: QThread::currentThreadId = " << Qt::dec << mainQtId
        << ", pthread_self = " << Qt::dec << quintptr(mainPtId);

    std::cout << "Main thread: std::this_thread::get_id = "
              << std::this_thread::get_id() << std::endl;

    // --- QThread path ---
    QThread qtThread;
    QtWorker worker;
    worker.moveToThread(&qtThread);

    QObject::connect(&qtThread, &QThread::started,
                     &worker, &QtWorker::doWork);

    QObject::connect(&qtThread, &QThread::finished, []() {
        qDebug() << "[QtThread] finished signal received.";
    });

    qtThread.start();

    // --- std::jthread path ---
    std::jthread jthr([] {
        auto qtId = reinterpret_cast<quintptr>(QThread::currentThreadId());
        auto ptId = pthread_self();

        qDebug().nospace()
            << "[std::jthread] QThread::currentThreadId = " << Qt::dec << qtId
            << ", pthread_self = " << Qt::dec << quintptr(ptId);

        std::cout << "[std::jthread] std::this_thread::get_id = "
                  << std::this_thread::get_id() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(700));
        qDebug() << "[std::jthread] done.";
    });

    QTimer::singleShot(2000, &app, [&] {
        qDebug() << "[main] quitting event loop.";
        qtThread.quit();
        app.quit();
    });

    int rc = app.exec();

    if (qtThread.isRunning()) {
        qtThread.quit();
        qtThread.wait();
    }

    qDebug() << "[main] app.exec() returned" << rc;
    return rc;
}

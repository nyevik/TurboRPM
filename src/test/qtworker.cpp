#include "qtworker.h"

#include <QThread>
#include <QDebug>

#include <thread>
#include <chrono>
#include <pthread.h>
#include <iostream>

void QtWorker::doWork()
{
    auto qtId = reinterpret_cast<quintptr>(QThread::currentThreadId());
    auto ptId = pthread_self();

    // QDebug only sees integers/pointers
    qDebug().nospace()
        << "[QtWorker] QThread::currentThreadId = " << Qt::dec << qtId
        << ", pthread_self = " << Qt::dec << quintptr(ptId);

    // std::cout handles std::thread::id
    std::cout << "[QtWorker] std::this_thread::get_id = "
              << std::this_thread::get_id() << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    qDebug() << "[QtWorker] done.";
}

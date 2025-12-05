#pragma once

#include <QObject>

class QtWorker : public QObject
{
    Q_OBJECT
public slots:
    void doWork();
};

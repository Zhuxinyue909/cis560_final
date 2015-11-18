#pragma once

#include <QtCore>
#include <QDebug>
#include <QTimer>

class MyTimer : public QTimer
{
    Q_OBJECT
public:
    MyTimer();
    long time_elapsed();
private:
    long ms;
public slots:
    void add_time();
};


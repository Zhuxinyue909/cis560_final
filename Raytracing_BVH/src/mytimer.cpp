#include "mytimer.h"

MyTimer::MyTimer()
{
    //connect(this, SIGNAL(timeout()), this, SLOT(add_time()));
    ms = 0;
}

long MyTimer::time_elapsed()
{
    return ms;
}

void MyTimer::add_time()
{
    ms = ms+1;
    qDebug() << "slot called";
}

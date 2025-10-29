#include "fpscollector.h"

FPSCollector::FPSCollector(QObject *parent)
    : QObject{parent}
{
    timer = new QElapsedTimer();
}

void FPSCollector::start()
{
    isRunning = true;
    timer->start();
}

void FPSCollector::stop()
{
    isRunning = false;
}

void FPSCollector::addFrame()
{
    if (!isRunning) return;

    qint64 now = timer->elapsed();
    if (lastTime == 0)
    {
        lastTime = now;
        return;
    }
    double fps = 1000.0 / (double)(now - lastTime + 0.0001);
    lastTime = now;
    emit fpsUpdated(fps);
}

FPSCollector::~FPSCollector()
{
    delete timer;
}

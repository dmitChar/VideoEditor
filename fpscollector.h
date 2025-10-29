#ifndef FPSCOLLECTOR_H
#define FPSCOLLECTOR_H

#include <QObject>
#include <QElapsedTimer>

class FPSCollector : public QObject
{
    Q_OBJECT
public:
    explicit FPSCollector(QObject *parent = nullptr);
    void start();
    void stop();
    void addFrame();
    ~FPSCollector();
signals:
    void fpsUpdated(double fps);

private:
    QElapsedTimer *timer;
    qint64 lastTime = 0;
    std::atomic<bool> isRunning;
};

#endif // FPSCOLLECTOR_H

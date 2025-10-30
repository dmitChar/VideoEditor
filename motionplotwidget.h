#ifndef MOTIONPLOTWIDGET_H
#define MOTIONPLOTWIDGET_H

#include<QChartView>
#include <QSplineSeries>
#include <QValueAxis>
#include <QElapsedTimer>
#include <atomic>

class MotionPlotWidget : public QChartView
{
public:
    MotionPlotWidget();
    void start();
    void stop();
    bool addPoint(float fps);

private:
    QChart *chart;
    QSplineSeries *series;
    QElapsedTimer timer;
    QValueAxis *axisX, *axisY;
    qint64 ySizeCup= 90;
    const int window = 10; //секунд
    const int maxPoints = 200;
    std::atomic<bool> isRunning = false;
    QVector<double> fpsHistory;
};

#endif // MOTIONPLOTWIDGET_H

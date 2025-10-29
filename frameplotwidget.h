#ifndef FRAMEPLOTWIDGET_H
#define FRAMEPLOTWIDGET_H

#include<QChartView>

#include <QSplineSeries>
#include <QValueAxis>
#include <QElapsedTimer>
#include <atomic>

class FramePlotWidget : public QChartView
{
public:
    explicit FramePlotWidget(QWidget *parent = nullptr);
    void start();
    void stop();
    bool addPoint(float fps);

private:
    QChart *chart;
    QSplineSeries *series;
    QElapsedTimer timer;
    QValueAxis *axisX, *axisY;
    qint64 ySizeCup= 90;
    const int maxPoints = 500;
    int xSizeCup = 12000;
    std::atomic<bool> isRunning = false;
    QVector<double> fpsHistory;
};

#endif // FRAMEPLOTWIDGET_H

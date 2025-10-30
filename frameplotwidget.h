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

    int ySizeCup= 45;
    const float window = 10;
    const int maxPoints = 200;

    std::atomic<bool> isRunning = false;
    QVector<double> fpsHistory;
};

#endif // FRAMEPLOTWIDGET_H

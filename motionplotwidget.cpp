#include "motionplotwidget.h"

MotionPlotWidget::MotionPlotWidget() {

    fpsHistory.reserve(maxPoints);
    chart = new QChart();
    series = new QSplineSeries();

    axisX = new QValueAxis();
    axisY = new QValueAxis();

    axisY->setRange(0.0, 1.0);
    axisX->setRange(0, window); // 10 секунд

    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chart->setTitle("FPS Monitor");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->hide();

    setRenderHint(QPainter::Antialiasing);
    setChart(chart);
    setMinimumHeight(300);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setFrameShape(QFrame::NoFrame);
    setContentsMargins(0, 0, 0, 0);
    setMinimumWidth(400);

}

void MotionPlotWidget::start()
{
    isRunning = true;
    timer.start();
    chart->setTitle("Motion Monitor (running)");
}

void MotionPlotWidget::stop()
{
    isRunning = false;
    series->clear();
    fpsHistory.clear();
    chart->setTitle("Motion Monitor (stopped)");
}

bool MotionPlotWidget::addPoint(float motionRate)
{
    if (!isRunning) return false;

    fpsHistory.push_back(motionRate);
    if (fpsHistory.size() > maxPoints)
        fpsHistory.pop_front();

    if (fpsHistory.isEmpty()) return false;


    double currentTime = timer.elapsed() / 1000.0;
    series->append(currentTime, motionRate);

    double minX = std::max(0.0, currentTime - window);
    axisX->setRange(minX, currentTime);

    // while (!series->points().isEmpty() && series->at(0).x() < minX + window + 1)
    // {
    //     series->remove(0);
    // }

    return true;

}

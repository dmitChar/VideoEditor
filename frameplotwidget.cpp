#include "frameplotwidget.h"
#include <algorithm>

FramePlotWidget::FramePlotWidget(QWidget *parent)
    : QChartView(parent)
{
    fpsHistory.reserve(maxPoints);
    chart = new QChart();
    series = new QSplineSeries();

    axisX = new QValueAxis();
    axisY = new QValueAxis();

    axisY->setRange(15, 45);
    axisX->setRange(0, 10); // 10 секунд

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
}

void FramePlotWidget::start()
{
    isRunning = true;
    timer.start();
    chart->setTitle("FPS Monitor (running)");
}

void FramePlotWidget::stop()
{
    isRunning = false;
    series->clear();
    fpsHistory.clear();
    chart->setTitle("FPS Monitor (stopped)");
}

bool FramePlotWidget::addPoint(float fps)
{
    if (!isRunning) return false;

    fpsHistory.push_back(fps);
    if (fpsHistory.size() > maxPoints)
        fpsHistory.pop_front();

    if (fpsHistory.isEmpty()) return false;

    double minY = *std::min_element(fpsHistory.begin(), fpsHistory.end());
    double maxY = *std::max_element(fpsHistory.begin(), fpsHistory.end());
    //axisY->setRange(std::max(0.0, minY - 5), maxY + 5);

    double currentTime = timer.elapsed() / 1000.0;
    series->append(currentTime, fps);

    double window = 10.0; // секунды
    double minX = std::max(0.0, currentTime - window);
    axisX->setRange(minX, currentTime);

    // while (!series->points().isEmpty() && series->at(0).x() < minX + window + 1)
    // {
    //     series->remove(0);
    // }

    return true;
}

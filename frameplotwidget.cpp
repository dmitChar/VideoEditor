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

    axisY->setRange(10, ySizeCup);
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

    lastUpdate = QDateTime::currentMSecsSinceEpoch();

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
    if (fpsHistory.isEmpty()) return false;
    now = QDateTime::currentMSecsSinceEpoch();

    // if (now - lastUpdate > window + 1 && fpsHistory.size() > maxPoints + 50)
    // {
    //     qDebug() << "Размер до удаления " << fpsHistory.size();
    //     lastUpdate = now;
    //     fpsHistory.erase(fpsHistory.begin(), fpsHistory.begin() + 50);
    //     qDebug() << "Размер после удаления " << fpsHistory.size();
    // }


    double minY = *std::min_element(fpsHistory.begin(), fpsHistory.end());
    double maxY = *std::max_element(fpsHistory.begin(), fpsHistory.end());
    axisY->setRange(minY, maxY);


    double currentTime = timer.elapsed() / 1000.0;
    double minX = std::max(0.0, currentTime - window);
    axisX->setRange(minX, currentTime);


    series->append(currentTime, fps);

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    // if (now - lastUpdate > 15000)
    // {
    //     lastUpdate = now;
    //     int removeCount = 0;
    //     auto points = series->points();

    //     for (auto &point : points)
    //     {
    //         if (point.x() < minX)
    //             removeCount++;
    //     }

    //     if (removeCount > 0)
    //     {
    //         series->removePoints(0, removeCount);
    //         qDebug() << "Удалены " << removeCount <<"элементов";
    //     }
    // }


    return true;
}

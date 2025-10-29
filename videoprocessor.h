// videoprocessor.h
#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QObject>
#include <QImage>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <cvutils.h>

class VideoProcessor : public QObject
{
    Q_OBJECT

public:
    explicit VideoProcessor(QObject *parent = nullptr);
    ~VideoProcessor();

private:
    cv::Mat prev_frame;
    bool showMotion = true;
    cv::Mat background;

    // Вспомогательная функция (перенесена из mainwindow)
    QImage matToImage(const cv::Mat &mat);

public slots:
    // Слот для получения кадра от MainWindow
    void processFrame(cv::Mat frame);

    // Слот для обновления флага
    void setShowMotion(bool show);

signals:
    // Сигнал, отправляющий результат обратно в MainWindow
    void frameProcessed(const QImage &processedImage, double motionLevel);
};

#endif // VIDEOPROCESSOR_H

// videoprocessor.cpp
#include "videoprocessor.h"

VideoProcessor::VideoProcessor(QObject *parent) : QObject(parent)
{
    bgSubstr = cv::createBackgroundSubtractorKNN(100, 40, 1);
}

VideoProcessor::~VideoProcessor()
{
    // Деструктор
}

// Этот слот будет выполняться в отдельном потоке
void VideoProcessor::processFrame(cv::Mat frame)
{
    if (frame.empty()) return;

    cv::Mat gray, mask;
    cv::Mat frameCopy = frame.clone();

    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5,5), 0);

    bgSubstr->apply(gray, mask);

    cv::erode(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);
    cv::dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);

    float motionLevel = cv::sum(mask)[0] / (frame.cols * frame.rows * 255.0);


    if (showMotion && motionLevel > 0.01)
    {
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


        for (const auto &contour : contours)
        {
            //qDebug() << cv::contourArea(contour);
            if (cv::contourArea(contour) < 400) continue;

            cv::Rect rect = cv::boundingRect(contour);
            if (rect.width < 20 || rect.height < 20) continue;

            cv::rectangle(frameCopy, rect, cv::Scalar(0, 255, 0), 2);
        }

    }

    // Конвертируем в QImage
    QImage processedImage = CVUtils::matToImage(frameCopy);

    // Отправляем результат обратно в GUI-поток
    emit frameProcessed(processedImage, motionLevel);
}

void VideoProcessor::setShowMotion(bool show)
{
    this->showMotion = show;
}

// Вспомогательная функция

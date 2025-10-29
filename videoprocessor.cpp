// videoprocessor.cpp
#include "videoprocessor.h"

VideoProcessor::VideoProcessor(QObject *parent) : QObject(parent)
{
    // Конструктор
}

VideoProcessor::~VideoProcessor()
{
    // Деструктор
}

// Этот слот будет выполняться в отдельном потоке
void VideoProcessor::processFrame(cv::Mat frame)
{
    if (frame.empty()) return;

    cv::Mat gray, diff, thresh;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

    if (background.empty())
        gray.convertTo(background, CV_32F);

    cv::accumulateWeighted(gray, background, 0.05);

    double motionLevel = 0.0;
    cv::Mat frameCopy = frame.clone();

    if (!prev_frame.empty())
    {
        //разница между текущим кадром и фоном
        cv::Mat background8U;
        background.convertTo(background8U, CV_8U);
        cv::absdiff(gray, background8U, diff);

        //порог
        cv::threshold(diff, thresh, 30, 255, cv::THRESH_BINARY);
        cv::erode(thresh, thresh, cv::Mat(), cv::Point(-1, -1), 2);
        cv::dilate(thresh, thresh, cv::Mat(), cv::Point(-1, -1), 2);

        motionLevel = cv::sum(thresh)[0] / (frame.cols * frame.rows * 255.0);

        if (showMotion && motionLevel > 0.01)
        {
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            std::vector<std::vector<cv::Point>> approxContours(contours.size());


            for (int i = 0; i < contours.size(); ++i)
            {
                if (cv::contourArea(contours[i]) < 100) continue;
                float peri = cv::arcLength(contours[i], 1);

                cv::approxPolyDP(contours[i], approxContours[i], peri * 0.02, 1);

                cv::Rect rect = cv::boundingRect(approxContours[i]);
                if (rect.width < 10 || rect.height < 10) continue;

                cv::rectangle(frameCopy, rect, cv::Scalar(0, 255, 0), 2);
            }

        }
    }

    prev_frame = gray;

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

#ifndef ABSTRACTUI_H
#define ABSTRACTUI_H

#include <QWidget>
#include <opencv2/opencv.hpp>

class AbstractUI : public QWidget
{
    Q_OBJECT
public:
    virtual bool open(const QString &path) = 0;
    virtual bool readFrame(const cv::Mat &frame) = 0;
    virtual bool isOpened() const = 0;
    virtual void release() = 0;

    virtual ~AbstractUI() = default;

signals:
};

#endif // ABSTRACTUI_H

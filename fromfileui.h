#ifndef FROMFILEUI_H
#define FROMFILEUI_H

#include "abstractui.h"

class FromFileUI : public AbstractUI
{
public:
    FromFileUI();
    bool open(const QString &path) override
    {
        return cap.open(path.toStdString());
    }

    bool readFrame(const cv::Mat &frame) override
    {
        return cap.read(frame);
    }
    bool isOpened() const override
    {
        return cap.isOpened();
    }
    void release() override
    {
        cap.release();
    }

private:
    cv::VideoCapture cap;
};

#endif // FROMFILEUI_H

#ifndef FROMCAMUI_H
#define FROMCAMUI_H

#include "abstractui.h"

class FromCamUI : public AbstractUI
{
public:
    FromCamUI();
    bool open(const QString &path = QString()) override
    {
        return cap.open(0);
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

#endif // FROMCAMUI_H

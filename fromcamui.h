#ifndef FROMCAMUI_H
#define FROMCAMUI_H

#include "abstractui.h"

class FromCamUI : public AbstractUI
{
public:
    FromCamUI();
    ~FromCamUI() override;
    bool open(const QString &path = QString()) override;
    bool read(cv::Mat &frame) override;
    bool isOpened() const override;
    void release() override;
private:
    cv::VideoCapture cap;

};

#endif // FROMCAMUI_H

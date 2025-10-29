#ifndef FROMFILEUI_H
#define FROMFILEUI_H

#include "abstractui.h"

class FromFileUI : public AbstractUI
{
public:
    FromFileUI();
    ~FromFileUI() override;

    bool open(const QString &path) override;
    bool read(cv::Mat &frame) override;
    bool isOpened() const override;
    void release() override;

private:
    cv::VideoCapture cap;
};
#endif // FROMFILEUI_H

#include "fromfileui.h"

FromFileUI::FromFileUI() {}
FromFileUI::~FromFileUI() { release(); }


bool FromFileUI::open(const QString &path)
{
    return cap.open(path.toStdString());
}

bool FromFileUI::read(cv::Mat &frame)
{
    return cap.read(frame);
}

bool FromFileUI::isOpened() const
{
    return cap.isOpened();
}

void FromFileUI::release()
{
    cap.release();
}

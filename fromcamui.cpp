#include "fromcamui.h"

FromCamUI::FromCamUI() {}

FromCamUI::~FromCamUI() {}


bool FromCamUI::open(const QString &path)
{
    return cap.open(0);
}

bool FromCamUI::read(cv::Mat &frame)
{
    return cap.read(frame);
}
bool FromCamUI::isOpened() const
{
    return cap.isOpened();
}
void FromCamUI::release()
{
    cap.release();
}

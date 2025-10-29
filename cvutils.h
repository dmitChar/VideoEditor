#ifndef CVUTILS_H
#define CVUTILS_H

#include <QImage>
#include <opencv2/opencv.hpp>

class CVUtils
{
public:
    CVUtils();
    static QImage matToImage(const cv::Mat &mat);
};

#endif // CVUTILS_H

#include "HazardDetector.hpp"
#include <iostream>

namespace hazard_detector
{
    HazardDetector::HazardDetector(const Config& nConfig)
        : config(nConfig)
    {
    }

    bool HazardDetector::analyze(std::vector<float> &distImage, std::pair<uint16_t, uint16_t> distDims, cv::Mat &visualImage)
    {
        int cn = visualImage.channels();
        uint8_t* pixelPtr = (uint8_t*)visualImage.data;

        std::pair< bool, cv::Mat > res;

        const uint16_t distHeight = distDims.first;
        const uint16_t distWidth = distDims.second;

        for (int i=0; i < visualImage.rows && i < distHeight; i++)
        {
            for (int j=0; j < visualImage.cols && j < distWidth; j++)
            {
                // mark everything under consideration green
                if (!std::isnan(calibration[i][j]))
                {
                    pixelPtr[i*visualImage.cols*cn + j*cn + 1] = 255;
                }
                // mark objects/pixels which are Xcm closer than calibration
                if (distImage[i*distWidth + j] < (calibration[i][j] - 0.2))
                {
                    pixelPtr[i*visualImage.cols*cn + j*cn + 0] = 255;
                    pixelPtr[i*visualImage.cols*cn + j*cn + 1] = 0;
                }
            }
        }

        return true;
    }

    void HazardDetector::setCalibration( std::vector< std::vector<float> > calibration )
    {
        this->calibration = calibration;
    }
}

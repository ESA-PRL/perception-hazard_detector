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

        //const uint16_t distHeight = distDims.first;
        const uint16_t distWidth  = distDims.second;

        for (int i=config.mask.minY; i < visualImage.rows && i < config.mask.maxY; i++)
        {
            for (int j=config.mask.minX; j < visualImage.cols && j < config.mask.maxX; j++)
            {
                // mark everything under consideration green
                if (!std::isnan(calibration[i][j]))
                {
                    pixelPtr[i*visualImage.cols*cn + j*cn + 1] = 255;
                }
                // mark objects/pixels which are Xcm closer than calibration
                if (distImage[i*distWidth + j] < (calibration[i][j] - config.tolerance))
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

        // if config.mask.* == -1, derive min/max from the distance image/calibration
        config.mask.maxX = config.mask.maxX == -1 ? (int)(calibration[0].size()) : config.mask.maxX;
        config.mask.maxY = config.mask.maxY == -1 ? (int)(calibration.size())    : config.mask.maxY;
        config.mask.minX = std::max(config.mask.minX, 0);
        config.mask.minY = std::max(config.mask.minY, 0);
    }
}

#include "HazardDetector.hpp"
#include <iostream>

namespace hazard_detector
{
    HazardDetector::HazardDetector(const Config& nConfig)
        : config(nConfig), calibrated(false)
    {
    }

    bool HazardDetector::analyze(std::vector<float> &distImage, std::pair<uint16_t, uint16_t> distDims, cv::Mat &visualImage)
    {
        int cn = visualImage.channels();
        uint8_t* pixelPtr = (uint8_t*)visualImage.data;

        int hazardPixels = 0;

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

                    hazardPixels++;
                }
            }
        }

        // spurious hazard?
        if (hazardPixels >= config.hazardPixelLimit)
            return true;

        return false;
    }

    bool HazardDetector::setCalibration( std::vector< std::vector<float> > calibration )
    {
        this->calibration = calibration;
        return calculateMask();
    }

    bool HazardDetector::readCalibrationFile( std::string path )
    {
        std::ifstream file(path);

        std::string line;
        while( std::getline(file, line) )
        {
            std::stringstream linestream(line);
            std::string entry;
            std::vector< float > rowData;
            while( std::getline(linestream, entry, ',') )
            {
                rowData.push_back(std::stof(entry));
            }
            calibration.push_back(rowData);
        }
        file.close();

        return calculateMask();
    }

    bool HazardDetector::calculateMask()
    {
        if (calibration.size() == 0 || calibration[0].size() == 0)
            return false;

        // if config.mask.* == -1, derive min/max from the distance image/calibration
        config.mask.maxX = config.mask.maxX < 0 ? (int)(calibration[0].size()) : config.mask.maxX;
        config.mask.maxY = config.mask.maxY < 0 ? (int)(calibration.size())    : config.mask.maxY;
        config.mask.minX = std::max(config.mask.minX, 0);
        config.mask.minY = std::max(config.mask.minY, 0);

        calibrated = true;

        return true;
    }

    bool HazardDetector::saveCalibrationFile( std::string path )
    {
        std::ofstream file(path);
        if (!file)
            return false;

        for (unsigned int i=0; i < calibration.size(); i++)
        {
            for (unsigned int j=0; j < calibration[0].size(); j++)
            {
                file << calibration[i][j] << ",";
            }
            file << std::endl;
        }
        file.flush();
        file.close();

        return true;
    }

    bool HazardDetector::isCalibrated()
    {
        return calibrated;
    }
}

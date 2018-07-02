#include "HazardDetector.hpp"
#include <iostream>

namespace hazard_detector
{
    HazardDetector::HazardDetector(const Config& nConfig)
        : config(nConfig), calibrated(false)
    {
        HAZARD = 255;
        TRAVERSABLE = 0;

        trav_map.resize(getTravMapWidth()*getTravMapHeight(), TRAVERSABLE);
    }

    bool HazardDetector::analyze(std::vector<float> &distImage, std::pair<uint16_t, uint16_t> distDims, cv::Mat &visualImage)
    {
        int cn = visualImage.channels();
        uint8_t* pixelPtr = (uint8_t*)visualImage.data;

        int hazardPixels = 0;

        //const uint16_t distHeight = distDims.first;
        const uint16_t distWidth  = distDims.second;

        // reset traversability map
        trav_map.clear();
        trav_map.resize(getTravMapWidth()*getTravMapHeight(), TRAVERSABLE);

        for (int i=config.mask.minY; i < visualImage.rows && i < config.mask.maxY; i++)
        {
            for (int j=config.mask.minX; j < visualImage.cols && j < config.mask.maxX; j++)
            {
                int original_green_value = pixelPtr[i*visualImage.cols*cn + j*cn + 1];

                float distance = distImage[i*distWidth + j];
                float calibration_distance = calibration[i][j];

                // reject artifacts and invalid regions
                if (std::isnan(calibration_distance) || std::isnan(distance) || distance <= 0.0)
                    continue;

                // mark everything under consideration green
                pixelPtr[i*visualImage.cols*cn + j*cn + 0] /= 1.1;
                pixelPtr[i*visualImage.cols*cn + j*cn + 1] = 255;
                pixelPtr[i*visualImage.cols*cn + j*cn + 2] /= 1.1;

                // mark objects/pixels which are Xcm closer than calibration
                if (distance < (calibration_distance - config.tolerance_close))
                {
                    pixelPtr[i*visualImage.cols*cn + j*cn + 0] = 255;
                    pixelPtr[i*visualImage.cols*cn + j*cn + 1] = original_green_value;
                    pixelPtr[i*visualImage.cols*cn + j*cn + 2] /= 1.5;

                    registerHazard(i,j);

                    hazardPixels++;
                }

                // mark objects/pixels which are Xcm further away than calibration
                if (distance > (calibration_distance + config.tolerance_far))
                {
                    pixelPtr[i*visualImage.cols*cn + j*cn + 0] /= 1.5;
                    pixelPtr[i*visualImage.cols*cn + j*cn + 1] = original_green_value;
                    pixelPtr[i*visualImage.cols*cn + j*cn + 2] = 255;

                    registerHazard(i,j);

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

    bool HazardDetector::registerHazard(int row, int col)
    {
        int n_rows = config.mask.maxY - config.mask.minY;
        int n_cols = config.mask.maxX - config.mask.minX;

        row -= config.mask.minY;
        col -= config.mask.minX;

        double scaling_r = (config.dist_to_upper_edge - config.dist_to_bottom_edge)/(double)n_rows;
        double scaling_c = (fabs(config.dist_to_left_edge)   + fabs(config.dist_to_right_edge))/(double)n_cols;

        // find indices, pretending rover is at (0,0)
        int ind_r = (int)( ( -config.dist_to_bottom_edge + row*scaling_r ) / config.trav_map.resolution );
        int ind_c = (int)( (  config.dist_to_left_edge   + col*scaling_c ) / config.trav_map.resolution );

        // move reference rover to the center of the traversability map
        ind_r += getTravMapHeight() / 2;
        ind_c += getTravMapWidth() / 2;

        trav_map[ind_r*getTravMapWidth() + ind_c] = HAZARD;

        return true;
    }

    const std::vector<uint8_t>& HazardDetector::getTraversabilityMap()
    {
        return trav_map;
    }

    int HazardDetector::getTravMapWidth()
    {
        int width = (int)(config.trav_map.width/config.trav_map.resolution);
        return width + 1 - width%2;
    }

    int HazardDetector::getTravMapHeight()
    {
        int height = (int)(config.trav_map.height/config.trav_map.resolution);
        return height + 1 - height%2;
    }
}

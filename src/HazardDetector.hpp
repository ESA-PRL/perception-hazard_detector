#ifndef _HAZARD_DETECTOR_HPP_
#define _HAZARD_DETECTOR_HPP_

#include <hazard_detector/Config.hpp>
#include <vector>
#include <stdint.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <cmath>

namespace hazard_detector
{
    class HazardDetector
    {
        public:
            HazardDetector(const Config& config = Config());

            bool analyze
                (
                    std::vector<float> &distImage,
                    std::pair<uint16_t,uint16_t> distDims,
                    cv::Mat &visualImage
                );

            bool setCalibration( std::vector< std::vector<float> > calibration );
            bool readCalibrationFile( std::string path );
            bool saveCalibrationFile( std::string path );
            bool isCalibrated();
            const std::vector<uint8_t> &getTraversabilityMap();
            int getTravMapDims();

            uint8_t HAZARD;
            uint8_t TRAVERSABLE;

        private:
            Config config;
            std::vector< std::vector<float> > calibration;
            bool calculateMask();
            bool calibrated;
            std::vector<uint8_t> trav_map;
            bool registerHazard(int,int);
    };

}

#endif //_HAZARD_DETECTOR_HPP_

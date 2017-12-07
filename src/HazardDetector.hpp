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

        private:
            Config config;
            std::vector< std::vector<float> > calibration;
            bool calculateMask();
    };

}

#endif //_HAZARD_DETECTOR_HPP_

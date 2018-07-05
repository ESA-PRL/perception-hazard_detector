#ifndef _HAZARD_DETECTOR_HPP_
#define _HAZARD_DETECTOR_HPP_

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include <hazard_detector/Config.hpp>

namespace hazard_detector
{
    class HazardDetector
    {
        public:
            HazardDetector(const Config& config = Config());

            bool analyze
                (
                    std::vector<float>& distance_image,
                    std::pair<uint16_t,uint16_t> distance_dims,
                    cv::Mat& visual_image
                );

            bool setCalibration(std::vector< std::vector<float> > calibration);
            bool readCalibrationFile(std::string path);
            bool saveCalibrationFile(std::string path);

            bool isCalibrated() const;
            std::vector<uint8_t> const& getTraversabilityMap();
            int getTravMapWidth() const;
            int getTravMapHeight() const;

            uint8_t getValueForHazard() const;
            uint8_t getValueForTraversable() const;

        protected:
            Config config;
            std::vector< std::vector<float> > calibration;
            bool calibrated;
            std::vector<uint8_t> trav_map;
            cv::Mat transformation_matrix;
            bool found_transformation_matrix;

            uint8_t HAZARD;
            uint8_t TRAVERSABLE;

            bool calculateMask();
            cv::Point2f distancesToMapCoordinates(const double x, const double y) const;
            void computeTransformationMatrix();
    };

}

#endif //_HAZARD_DETECTOR_HPP_

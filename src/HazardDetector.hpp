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
                    const std::vector<float>& distance_image,
                    const std::pair<uint16_t,uint16_t> distance_dims,
                    const cv::Mat& visual_image
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

            unsigned int getHazardPixelLimit() const;

            void computeTolerances();

            // these three functions can be used to ignore part of the RoI
            // in case we want to avoid an obstacle in Ackermann mode
            void ignoreLeftSide();
            void ignoreRightSide();
            void ignoreNothing();

        protected:
            Config config;
            std::vector< std::vector<float> > calibration;
            std::vector< std::vector<float> > max_tolerated_distances;
            std::vector< std::vector<float> > min_tolerated_distances;
            bool calibrated;
            std::vector<uint8_t> trav_map;
            cv::Mat transformation_matrix;
            bool found_transformation_matrix;

            // used to adjust which part of the RoI we consider
            // via, e.g., ignoreLeftSide()
            int left_most_hazard;
            int right_most_hazard;
            int min_x;
            int max_x;

            uint8_t HAZARD;
            uint8_t TRAVERSABLE;

            bool calculateRegionOfInterest();
            cv::Point2f distancesToMapCoordinates(const double x, const double y) const;
            void computeTransformationMatrix();

            float computeMinToleratedDistance(const float) const;
            float computeMaxToleratedDistance(const float) const;
    };

}

#endif //_HAZARD_DETECTOR_HPP_

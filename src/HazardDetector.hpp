#ifndef _HAZARD_DETECTOR_HPP_
#define _HAZARD_DETECTOR_HPP_

#include <hazard_detector/Config.hpp>
#include <vector>
#include <stdint.h>
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

        private:
            Config config;
    };

}

#endif //_HAZARD_DETECTOR_HPP_

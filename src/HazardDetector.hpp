#ifndef _HAZARD_DETECTOR_HPP_
#define _HAZARD_DETECTOR_HPP_

#include <hazard_detector/Config.hpp>
#include <vector>
#include <stdint.h> // for uint8_t
#include <opencv2/opencv.hpp>

namespace hazard_detector
{

class HazardDetector
{
public:
    HazardDetector(const Config& config = Config());

    std::pair< bool, std::vector<uint8_t> > analyze(std::vector<float> distImage, std::vector<uint8_t> visualImage);

private:
    Config config;
};

}

#endif //_HAZARD_DETECTOR_HPP_

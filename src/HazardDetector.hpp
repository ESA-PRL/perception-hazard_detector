#ifndef _HAZARD_DETECTOR_HPP_
#define _HAZARD_DETECTOR_HPP_

#include <hazard_detector/Config.hpp>
#include <vector>
#include <stdint.h> // for uint8_t

namespace hazard_detector
{

class HazardDetector
{
public:
    HazardDetector(const Config& config = Config());

    int analyze(std::vector<uint8_t> image);

private:
    Config config;
};

}

#endif //_HAZARD_DETECTOR_HPP_

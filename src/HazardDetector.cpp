#include "HazardDetector.hpp"
#include <iostream>

namespace hazard_detector
{

    HazardDetector::HazardDetector(const Config& nConfig)
        : config(nConfig)
    {
    }

    bool HazardDetector::analyze(std::vector<uint8_t> image)
    {
        return false;
    }

}

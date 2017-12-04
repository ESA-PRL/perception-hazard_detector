#include "HazardDetector.hpp"
#include <iostream>

namespace hazard_detector
{

    HazardDetector::HazardDetector(const Config& nConfig)
        : config(nConfig)
    {
    }

    std::pair< bool, std::vector<uint8_t> > HazardDetector::analyze(std::vector<float> distImage, std::vector<uint8_t> visualImage)
    {
        std::pair< bool, std::vector<uint8_t> > res;

        std::cout << "dist image " << distImage.size() << std::endl;
        std::cout << "vis. image " << visualImage.size() << std::endl;

        res.first = true;
        res.second = visualImage;

        return res;
    }
}

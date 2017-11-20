#ifndef _HAZARD_DETECTOR_CONFIG_HPP_
#define _HAZARD_DETECTOR_CONFIG_HPP_

#include <stdint.h> // for uint8_t

namespace hazard_detector
{
    struct Config
    {
        double maxObstacleHeight; // [m]

        Config()
            :
            maxObstacleHeight(0.4)
        {
        }
    };
}

#endif //_HAZARD_DETECTOR_CONFIG_HPP_

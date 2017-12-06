#ifndef _HAZARD_DETECTOR_CONFIG_HPP_
#define _HAZARD_DETECTOR_CONFIG_HPP_

#include <stdint.h> // for uint8_t
#include <string>

namespace hazard_detector
{
    struct Mask
    {
        int minX;
        int maxX;
        int minY;
        int maxY;

        // choose values of -1 to select min/max derived from image
        Mask(int minX, int maxX, int minY, int maxY)
        {
            this->minX = minX; // x=0 is on the left
            this->maxX = maxX;
            this->minY = minY; // y=0 is in the top
            this->maxY = maxY;
        }
    };

    struct Config
    {
        double tolerance = 0.4; // [m]
        Mask mask;              // area of interest
        int hazardPixelLimit = 20;  // if less than this many pixels
                                    // are considered hazardous, we consider them spurious
        std::string calibrationPath;
        bool newCalibration = false;
        int numCalibrationSamples = 20;

        Config()
            :
            mask(50,950,350,750)
        {
        }
    };
}

#endif //_HAZARD_DETECTOR_CONFIG_HPP_

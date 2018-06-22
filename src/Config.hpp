#ifndef _HAZARD_DETECTOR_CONFIG_HPP_
#define _HAZARD_DETECTOR_CONFIG_HPP_

#include <stdint.h> // for uint8_t
#include <string>
#include <vector>

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

    struct TraversabilityMap
    {
        double width, height; // dimensions of traversability map [m]
        double resolution;

        TraversabilityMap(double width, double height, double resolution)
        {
            this->resolution = resolution;
            this->width = width;
            this->height = height;
        }
    };

    struct Config
    {
        double tolerance_close, tolerance_far;       // [m]
        Mask mask;              // area of interest
        int hazardPixelLimit;   // if less than this many pixels
                                // are considered hazardous, we consider them spurious
        std::string calibrationPath;
        bool newCalibration;
        int numCalibrationSamples;

        TraversabilityMap trav_map;

        /**
         * Distances from the rover's frame to the
         * left/right/bottom/upper edge of the
         * region of interest in meters.
         *
         * These values are needed to create a
         * traversability map.
         */
        double dist_to_left_edge;
        double dist_to_right_edge;
        double dist_to_bottom_edge;
        double dist_to_upper_edge;

        Config()
            :
            mask(50,950,350,750),
            trav_map(5,5,0.1)
        {
        }
    };
}

#endif //_HAZARD_DETECTOR_CONFIG_HPP_

#ifndef _HAZARD_DETECTOR_CONFIG_HPP_
#define _HAZARD_DETECTOR_CONFIG_HPP_

#include <stdint.h> // for uint8_t
#include <string>
#include <vector>

namespace hazard_detector
{
    struct RegionOfInterest
    {
        int min_x;
        int max_x;
        int min_y;
        int max_y;

        // Distances from the rover's frame to the
        // left/right/bottom/upper edge of the
        // region of interest in meters.
        // These values are needed to create a
        // traversability map.
        double dist_to_upper_left_x;
        double dist_to_upper_left_y;
        double dist_to_upper_right_x;
        double dist_to_upper_right_y;
        double dist_to_bottom_right_x;
        double dist_to_bottom_right_y;
        double dist_to_bottom_left_x;
        double dist_to_bottom_left_y;

        // Choose values of -1 to select min/max derived from image.
        // x=0 is on the left
        // y=0 is in the top
        RegionOfInterest(const int min_x, const int max_x, const int min_y, const int max_y)
        {
            this->min_x = min_x;
            this->max_x = max_x;
            this->min_y = min_y;
            this->max_y = max_y;
        }
    };

    struct TraversabilityMap
    {
        // dimensions of traversability map [m]
        double width, height;
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
        // tolerances [m]
        double tolerance_close;
        double tolerance_far;

        RegionOfInterest roi;

        TraversabilityMap trav_map;

        // If less than this many pixels are considered hazardous,
        // we consider them spurious.
        unsigned int hazard_pixel_limit;

        Config()
            :
            roi(50,950,350,750),
            trav_map(5,5,0.1)
        {
        }
    };
}

#endif //_HAZARD_DETECTOR_CONFIG_HPP_

#ifndef _HAZARD_DETECTOR_CONFIG_HPP_
#define _HAZARD_DETECTOR_CONFIG_HPP_

#include <stdint.h> // for uint8_t
#include <string>
#include <vector>

namespace hazard_detector
{
    struct Mask
    {
        int min_x;
        int max_x;
        int min_y;
        int max_y;

        // choose values of -1 to select min/max derived from image
        Mask(int min_x, int max_x, int min_y, int max_y)
        {
            this->min_x = min_x; // x=0 is on the left
            this->max_x = max_x;
            this->min_y = min_y; // y=0 is in the top
            this->max_y = max_y;
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
        double tolerance_close, tolerance_far; // [m]
        Mask mask;              // area of interest
        int hazard_pixel_limit; // if less than this many pixels
                                // are considered hazardous, we consider them spurious
        std::string calibration_path;
        bool new_calibration;
        int num_calibration_samples;

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

#include "hazard_detector/HazardDetector.hpp"

using namespace hazard_detector;

HazardDetector::HazardDetector(const Config& nConfig)
    : config(nConfig), calibrated(false)
{
    HAZARD = 255;
    TRAVERSABLE = 0;

    trav_map.resize(getTravMapWidth()*getTravMapHeight(), TRAVERSABLE);

    min_x = config.roi.min_x;
    max_x = config.roi.max_x;
    left_most_hazard = config.roi.min_x;
    right_most_hazard = config.roi.max_x;
}

bool HazardDetector::analyze(const std::vector<float>& dist_image, const std::pair<uint16_t, uint16_t> dist_dims, const cv::Mat& visual_image)
{
    if (!isCalibrated())
    {
        return false;
    }

    computeTransformationMatrix();

    int channels = visual_image.channels();
    uint8_t* image_data = static_cast<uint8_t*>(visual_image.data);

    std::vector<cv::Point2f> hazard_points;

    const uint16_t dist_width = dist_dims.second;

    // reset traversability map
    trav_map.clear();
    trav_map.resize(getTravMapWidth()*getTravMapHeight(), TRAVERSABLE);

    for (int i = config.roi.min_y; i < visual_image.rows && i < config.roi.max_y; ++i)
    {
        for (int j = min_x; j < visual_image.cols && j < max_x; ++j)
        {
            int index = i*visual_image.cols*channels + j*channels;
            int original_green_value = image_data[index + 1];
            float distance = dist_image[i*dist_width + j];
            float calibration_distance = calibration[i][j];

            // reject artifacts and invalid regions
            if (std::isnan(calibration_distance) || std::isnan(distance) || distance <= 0.25)
                continue;

            // mark everything under consideration green
            image_data[index + 0] /= 1.1;
            image_data[index + 1] = 255;
            image_data[index + 2] /= 1.1;

            // mark objects/pixels which are Xcm closer than calibration
            if (distance < min_tolerated_distances[i][j])
            {
                image_data[index + 0] = 255;
                image_data[index + 1] = original_green_value;
                image_data[index + 2] /= 1.5;

                hazard_points.push_back(cv::Point2f(j,i));
                left_most_hazard = std::min(left_most_hazard, j);
                right_most_hazard = std::max(right_most_hazard, j);
            }

            // mark objects/pixels which are Xcm further away than calibration
            if (distance > max_tolerated_distances[i][j])
            {
                image_data[index + 0] /= 1.5;
                image_data[index + 1] = original_green_value;
                image_data[index + 2] = 255;

                hazard_points.push_back(cv::Point2f(j,i));
                left_most_hazard = std::min(left_most_hazard, j);
                right_most_hazard = std::max(right_most_hazard, j);
            }
        }
    }

    // hazard area big enough? then transform hazard pixel coordinates to trav_map coordinates
    if (hazard_points.size() >= config.hazard_pixel_limit)
    {
        cv::perspectiveTransform(hazard_points, hazard_points, transformation_matrix);
        for (cv::Point2f p : hazard_points)
        {
            trav_map[static_cast<int>(p.y)*getTravMapWidth() + static_cast<int>(p.x)] = HAZARD;
        }
        return true;
    }

    return false;
}

bool HazardDetector::setCalibration(std::vector< std::vector<float> > calibration)
{
    this->calibration = calibration;
    return calculateRegionOfInterest();
}

bool HazardDetector::readCalibrationFile(std::string path)
{
    std::ifstream file(path);

    std::string line;
    while( std::getline(file, line) )
    {
        std::stringstream linestream(line);
        std::string entry;
        std::vector< float > row_data;
        while( std::getline(linestream, entry, ',') )
        {
            row_data.push_back(std::stof(entry));
        }
        calibration.push_back(row_data);
    }
    file.close();

    return calculateRegionOfInterest();
}

bool HazardDetector::calculateRegionOfInterest()
{
    if (calibration.size() == 0 || calibration[0].size() == 0)
        return false;

    // if config.roi.* == -1, derive min/max from the distance image/calibration
    config.roi.max_x = config.roi.max_x < 0 ? static_cast<int>(calibration[0].size()) : config.roi.max_x;
    config.roi.max_y = config.roi.max_y < 0 ? static_cast<int>(calibration.size())    : config.roi.max_y;
    config.roi.min_x = std::max(config.roi.min_x, 0);
    config.roi.min_y = std::max(config.roi.min_y, 0);

    calibrated = true;

    return true;
}

bool HazardDetector::saveCalibrationFile( std::string path )
{
    std::ofstream file(path);
    if (!file)
        return false;

    for (unsigned int i=0; i < calibration.size(); ++i)
    {
        for (unsigned int j=0; j < calibration[0].size(); ++j)
        {
            file << calibration[i][j] << ",";
        }
        file << std::endl;
    }
    file.flush();
    file.close();

    return true;
}

bool HazardDetector::isCalibrated() const
{
    return calibrated;
}

std::vector<uint8_t> const& HazardDetector::getTraversabilityMap()
{
    return trav_map;
}

int HazardDetector::getTravMapWidth() const
{
    int width = static_cast<int>(config.trav_map.width/config.trav_map.resolution);
    return width + 1 - width%2;
}

int HazardDetector::getTravMapHeight() const
{
    int height = static_cast<int>(config.trav_map.height/config.trav_map.resolution);
    return height + 1 - height%2;
}

uint8_t HazardDetector::getValueForHazard() const
{
    return HAZARD;
}

uint8_t HazardDetector::getValueForTraversable() const
{
    return TRAVERSABLE;
}

void HazardDetector::computeTransformationMatrix()
{
    static bool already_computed = false;

    if (already_computed)
        return;

    cv::Point2f src[4];
    cv::Point2f dst[4];

    src[0].x = config.roi.min_x;
    src[0].y = config.roi.min_y;
    src[1].x = config.roi.max_x;
    src[1].y = config.roi.min_y;
    src[2].x = config.roi.max_x;
    src[2].y = config.roi.max_y;
    src[3].x = config.roi.min_x;
    src[3].y = config.roi.max_y;

    dst[0] = distancesToMapCoordinates(config.roi.dist_to_upper_left_x,   config.roi.dist_to_upper_left_y);
    dst[1] = distancesToMapCoordinates(config.roi.dist_to_upper_right_x,  config.roi.dist_to_upper_right_y);
    dst[2] = distancesToMapCoordinates(config.roi.dist_to_bottom_right_x, config.roi.dist_to_bottom_right_y);
    dst[3] = distancesToMapCoordinates(config.roi.dist_to_bottom_left_x,  config.roi.dist_to_bottom_left_y);

    transformation_matrix = cv::getPerspectiveTransform(src, dst);

    already_computed = true;
}

cv::Point2f HazardDetector::distancesToMapCoordinates(const double x, const double y) const
{
    cv::Point2f p;
    p.x = x/config.trav_map.resolution + getTravMapWidth()/2.0;
    p.y = -y/config.trav_map.resolution + getTravMapHeight()/2.0;
    return p;
}

void HazardDetector::ignoreLeftSide()
{
    min_x = left_most_hazard;
    max_x = config.roi.max_x;
}

void HazardDetector::ignoreRightSide()
{
    min_x = config.roi.min_x;
    max_x = right_most_hazard;
}

void HazardDetector::ignoreNothing()
{
    min_x = config.roi.min_x;
    max_x = config.roi.max_x;
}

unsigned int HazardDetector::getHazardPixelLimit() const
{
    return config.hazard_pixel_limit;
}

float HazardDetector::computeMinToleratedDistance(const float calibration_distance) const
{
    double cos_alpha = config.camera_height / calibration_distance;
    return static_cast<float>((config.camera_height - config.tolerance_close) / cos_alpha);
}

float HazardDetector::computeMaxToleratedDistance(const float calibration_distance) const
{
    double cos_alpha = config.camera_height / calibration_distance;
    return static_cast<float>((config.camera_height + config.tolerance_far) / cos_alpha);
}

void HazardDetector::computeTolerances()
{
    if (!isCalibrated())
    {
        std::cerr << "ERROR in HAZARD DETECTOR is not calibrated yet, cannot compute tolerances!" << std::endl;
        std::exit(-1);
    }

    min_tolerated_distances.resize(calibration.size(), std::vector<float>(calibration[0].size(),nanf("")));
    max_tolerated_distances.resize(calibration.size(), std::vector<float>(calibration[0].size(),nanf("")));
    for (int i = config.roi.min_y; i < config.roi.max_y; ++i)
    {
        for (int j = config.roi.min_x; j < config.roi.max_x; ++j)
        {
            float distance = calibration[i][j];

            if (std::isnan(distance))
                continue;

            min_tolerated_distances[i][j] = computeMinToleratedDistance(distance);
            max_tolerated_distances[i][j] = computeMaxToleratedDistance(distance);
        }
    }
}

#include "HazardDetector.hpp"

using namespace hazard_detector;

HazardDetector::HazardDetector(const Config& nConfig)
    : config(nConfig), calibrated(false)
{
    HAZARD = 255;
    TRAVERSABLE = 0;

    found_transformation_matrix = false;

    trav_map.resize(getTravMapWidth()*getTravMapHeight(), TRAVERSABLE);
}

bool HazardDetector::analyze(std::vector<float> &dist_image, std::pair<uint16_t, uint16_t> dist_dims, cv::Mat &visual_image)
{
    if (!isCalibrated())
    {
        return false;
    }

    if (!found_transformation_matrix)
    {
        computeTransformationMatrix();
    }

    int cn = visual_image.channels();
    uint8_t* pixelPtr = (uint8_t*)visual_image.data;

    std::vector<cv::Point2f> hazard_points;

    //const uint16_t distHeight = dist_dims.first;
    const uint16_t dist_width  = dist_dims.second;

    // reset traversability map
    trav_map.clear();
    trav_map.resize(getTravMapWidth()*getTravMapHeight(), TRAVERSABLE);

    for (int i=config.mask.min_y; i < visual_image.rows && i < config.mask.max_y; i++)
    {
        for (int j=config.mask.min_x; j < visual_image.cols && j < config.mask.max_x; j++)
        {
            int original_green_value = pixelPtr[i*visual_image.cols*cn + j*cn + 1];

            float distance = dist_image[i*dist_width + j];
            float calibration_distance = calibration[i][j];

            // reject artifacts and invalid regions
            if (std::isnan(calibration_distance) || std::isnan(distance) || distance <= 0.25)
                continue;

            // mark everything under consideration green
            pixelPtr[i*visual_image.cols*cn + j*cn + 0] /= 1.1;
            pixelPtr[i*visual_image.cols*cn + j*cn + 1] = 255;
            pixelPtr[i*visual_image.cols*cn + j*cn + 2] /= 1.1;

            // mark objects/pixels which are Xcm closer than calibration
            if (distance < (calibration_distance - config.tolerance_close))
            {
                pixelPtr[i*visual_image.cols*cn + j*cn + 0] = 255;
                pixelPtr[i*visual_image.cols*cn + j*cn + 1] = original_green_value;
                pixelPtr[i*visual_image.cols*cn + j*cn + 2] /= 1.5;

                hazard_points.push_back(cv::Point2f(j,i));
            }

            // mark objects/pixels which are Xcm further away than calibration
            if (distance > (calibration_distance + config.tolerance_far))
            {
                pixelPtr[i*visual_image.cols*cn + j*cn + 0] /= 1.5;
                pixelPtr[i*visual_image.cols*cn + j*cn + 1] = original_green_value;
                pixelPtr[i*visual_image.cols*cn + j*cn + 2] = 255;

                hazard_points.push_back(cv::Point2f(j,i));
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
    return calculateMask();
}

bool HazardDetector::readCalibrationFile( std::string path )
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

    return calculateMask();
}

bool HazardDetector::calculateMask()
{
    if (calibration.size() == 0 || calibration[0].size() == 0)
        return false;

    // if config.mask.* == -1, derive min/max from the distance image/calibration
    config.mask.max_x = config.mask.max_x < 0 ? static_cast<int>(calibration[0].size()) : config.mask.max_x;
    config.mask.max_y = config.mask.max_y < 0 ? static_cast<int>(calibration.size())    : config.mask.max_y;
    config.mask.min_x = std::max(config.mask.min_x, 0);
    config.mask.min_y = std::max(config.mask.min_y, 0);

    calibrated = true;

    return true;
}

bool HazardDetector::saveCalibrationFile( std::string path )
{
    std::ofstream file(path);
    if (!file)
        return false;

    for (unsigned int i=0; i < calibration.size(); i++)
    {
        for (unsigned int j=0; j < calibration[0].size(); j++)
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
    cv::Point2f src[4];
    cv::Point2f dst[4];

    src[0].x = config.mask.min_x;
    src[0].y = config.mask.min_y;
    src[1].x = config.mask.max_x;
    src[1].y = config.mask.min_y;
    src[2].x = config.mask.max_x;
    src[2].y = config.mask.max_y;
    src[3].x = config.mask.min_x;
    src[3].y = config.mask.max_y;

    dst[0] = distancesToMapCoordinates(config.dist_to_upper_left_x,   config.dist_to_upper_left_y);
    dst[1] = distancesToMapCoordinates(config.dist_to_upper_right_x,  config.dist_to_upper_right_y);
    dst[2] = distancesToMapCoordinates(config.dist_to_bottom_right_x, config.dist_to_bottom_right_y);
    dst[3] = distancesToMapCoordinates(config.dist_to_bottom_left_x,  config.dist_to_bottom_left_y);

    transformation_matrix = cv::getPerspectiveTransform(src, dst);

    found_transformation_matrix = true;
}

cv::Point2f HazardDetector::distancesToMapCoordinates(const double x, const double y) const
{
    cv::Point2f p;
    p.x = x/config.trav_map.resolution + getTravMapWidth()/2.0;
    p.y = -y/config.trav_map.resolution + getTravMapHeight()/2.0;
    return p;
}

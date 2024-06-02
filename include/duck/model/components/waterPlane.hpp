#pragma once

#include <vector>


struct WaterPlane {
    float edge;

    std::vector<std::vector<float>> actHeightMap;
    std::vector<std::vector<float>> prevHeightMap;
};

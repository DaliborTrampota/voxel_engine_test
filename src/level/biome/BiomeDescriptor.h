#pragma once


#include <glm/glm.hpp>
#include <vector>

#include <Globals.h>
#include <random/Random.h>

#include "../Palette.h"


struct TerrainLayer {
    Palette palette;
    int depth;
};


struct BiomeDescriptor {
    std::vector<TerrainLayer> layers;

    float heightScale;
    float heightBase;
};


enum class ParameterRange {
    Low,
    Mid,
    High
};

struct Parameter {
    float min;
    float max;

    float distance(float val) const {
        float d1 = val - max;
        float d2 = min - val;
        return d1 > 0 ? d1 : glm::max(d2, 0.0f);
    }
};
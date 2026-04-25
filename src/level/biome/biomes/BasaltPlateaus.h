#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>
#include <random/PerlinNoise.h>

class BasaltPlateausBiome : public Biome {
  public:
    BasaltPlateausBiome()
        : Biome("BasaltPlateaus", {0.0f, 1.0f}, {-1.0f, 0.1f}, ParameterRange::Mid, 0) {
        auto& registry = engine::RegistryManager::Blocks();

        m_descriptor.heightScale = 6.0f;
        m_descriptor.heightBase = 63.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("basalt")->getID()}, {1}}, .depth = -1}
        );
    }
    float getHeightModifier(int x, int z, float baseHeight) const override {
        return Biome::getHeightModifier(x, z, baseHeight) + glm::round(m_noise.get2D(x, z) * 1.5f);
    }

  private:
    engine::PerlinNoise m_noise{1, 5, 0.0f, 1, 0.5f};
};
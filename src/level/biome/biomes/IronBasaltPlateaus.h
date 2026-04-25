#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>
#include <random/PerlinNoise.h>

class IronBasaltPlateausBiome : public Biome {
  public:
    IronBasaltPlateausBiome()
        : Biome("IronBasaltPlateaus", {-0.6f, 0.05f}, {-0.5f, 0.2f}, ParameterRange::High, 0) {
        auto& registry = engine::RegistryManager::Blocks();

        m_descriptor.heightScale = 6.0f;
        m_descriptor.heightBase = 65.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("iron_basalt")->getID()}, {1}}, .depth = -1}
        );
    }
    float getHeightModifier(int x, int z, float baseHeight) const override {
        return Biome::getHeightModifier(x, z, baseHeight) + glm::round(m_noise.get2D(x, z) * 1.5f);
    }

  private:
    engine::PerlinNoise m_noise{1, 5, 0.0f, 1, 0.5f};
};
#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>
#include <random/PerlinNoise.h>

class SilicateSandSeasBiome : public Biome {
  public:
    SilicateSandSeasBiome()
        : Biome("SilicateSandSeas", {0.25f, 1.0f}, {0.1f, 0.3f}, ParameterRange::Low, 10) {
        auto& registry = engine::RegistryManager::Blocks();

        m_descriptor.heightScale = 20.0f;
        m_descriptor.heightBase = 64.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("silicate_sand")->getID()}, {1}}, .depth = -1}
        );
    }

    float getHeightModifier(int x, int z, float baseHeight) const {
        float u = glm::dot(glm::vec2(x, z), m_windDirection);
        float ridges = 1.0 - abs(m_duneNoise.get2D(u * m_duneFreq, 0.0f));
        float dune = ridges < 0.5 ? ridges * 0.8       // windward
                                  : pow(ridges, 3.0);  // slip face

        float macro = m_duneNoise.get2D(x * 0.002, z * 0.002) * 0.5;
        float ripples = m_duneNoise.get2D(x * 0.05, z * 0.05) * 0.1;

        return Biome::getHeightModifier(x, z, baseHeight) + dune * m_duneHeight +
               macro * m_macroHeight + ripples * m_rippleHeight;
    }

  private:
    engine::PerlinNoise m_duneNoise{1, 5.0f, 0.0f, 3, 0.2f};
    glm::vec2 m_windDirection = glm::normalize(glm::vec2(1.0f, 0.3f));

    float m_duneHeight = 15;
    float m_macroHeight = 2;
    float m_rippleHeight = 0.5;
    float m_duneFreq = 0.01f;


    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};
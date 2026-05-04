#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>
#include <random/PerlinNoise.h>

class SilicateSandSeasBiome : public Biome {
  public:
    SilicateSandSeasBiome()
        : Biome("SilicateSandSeas", {0.4f, 1.0f}, {-1.0f, 0.5f}, ParameterRange::Low, 20) {
        auto& registry = engine::RegistryManager::Blocks();

        m_descriptor.heightScale = 2.0f;
        m_descriptor.heightBase = 62.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("silicate_sand")->getID()}, {1}}, .depth = -1}
        );
    }

    float getHeightModifier(int x, int z, float baseHeight) const {
        float u = glm::dot(glm::vec2(x, z), m_windDirection);
        float ridges = 1.0f - glm::abs(m_duneNoise.get2DNormalized(u * m_duneFreq, 0.0f));
        ridges = glm::clamp(ridges, 0.0f, 1.0f);
        float dune = ridges < 0.5f ? ridges * 0.8f         // windward
                                   : std::pow(ridges, 3.0f);  // slip face

        float macro = m_duneNoise.get2DNormalized(x * 0.002f, z * 0.002f);
        float ripples = m_duneNoise.get2DNormalized(x * 0.05f, z * 0.05f);

        return Biome::getHeightModifier(x, z, baseHeight) + dune * m_duneHeight +
               macro * m_macroHeight + ripples * m_rippleHeight;
    }

  private:
    // FastNoise SetScale(s) sets internal frequency = 1/s, so feature size is in
    // *input units*, not world blocks. We pre-scale coords by m_duneFreq below to
    // pick the actual world-space period.
    engine::PerlinNoise m_duneNoise{1, 1.0f, 0.0f, 3, 0.5f};
    glm::vec2 m_windDirection = glm::normalize(glm::vec2(1.0f, 0.3f));

    float m_duneHeight = 10;
    float m_macroHeight = 6;
    float m_rippleHeight = 0.6f;
    float m_duneFreq = 0.025f;  // period ≈ 1/freq = 40 blocks per dune crest


    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};
#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class MicrobialCrustBiome : public Biome {
  public:
    MicrobialCrustBiome()
        : Biome("MicrobialCrust", {-0.3f, -0.1f}, {-1.0f, -0.2f}, ParameterRange::Mid, 10) {
        auto& registry = engine::RegistryManager::Blocks();

        m_descriptor.heightScale = 8.0f;
        m_descriptor.heightBase = 64.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("microbial_matter")->getID()}, {1}}, .depth = 3}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("ash")->getID()}, {1}}, .depth = 6}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("slate")->getID()}, {1}}, .depth = -1}
        );
    }

    // float getHeightModifier(int x, int z, float baseHeight) const {
    //     // Add dune pattern
    //     float duneHeight = m_duneNoise.get(x, z) * 3.0f;
    //     return (baseHeight * m_descriptor.heightScale + duneHeight) + m_descriptor.heightBase;
    // }

    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};
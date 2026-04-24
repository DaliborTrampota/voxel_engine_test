#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include <block/Block.h>
#include <data/RegistryManager.h>
#include <level/ITerrainGenerator.h>
#include <random/PerlinNoise.h>

#include "biome/BiomeDescriptor.h"

class Biome;

namespace engine {
    class Chunk;
}

struct BiomeBlendData {
    Biome* biome;
    float weight;
};

class BiomeGenerator : public engine::ITerrainGenerator {
  public:
    BiomeGenerator(engine::PerlinNoise::Seed seed);

    engine::BlockID voxelAt(const glm::ivec3& pos) override;
    int height(int x, int z) const override;
    void populate(engine::Chunk& chunk) override;

    Biome* biomeAt(int x, int z) const;
    std::vector<BiomeBlendData> getBiomeBlend(int x, int z) const;

    void add(std::unique_ptr<Biome>&& biome);
    void setBlendRadius(float radius) { m_blendRadius = radius; }
    void setEnableBlending(bool enable) { m_enableBlending = enable; }

  protected:
    engine::PerlinNoise m_height;
    engine::PerlinNoise m_temperature;
    engine::PerlinNoise m_concentration;
    engine::PerlinNoise m_altitude;

    Parameter m_lowAltitude = {-1.0f, -0.15f};
    Parameter m_highAltitude = {0.35f, 1.0f};

    engine::RegistryManager::BlockRegistryT& m_blockRegistry;

    std::vector<std::unique_ptr<Biome>> m_biomes;

    float m_blendRadius = 8.0f;
    bool m_enableBlending = true;

  private:
    void paintColumns(engine::Chunk& chunk);
};

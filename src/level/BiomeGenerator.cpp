#include "BiomeGenerator.h"
#include "biome/Biome.h"

#include <Globals.h>
#include <data/RegistryManager.h>
#include <level/Chunk.h>
#include <random/Random.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

using namespace engine;

constexpr int worldScale = 10;

BiomeGenerator::BiomeGenerator(PerlinNoise::Seed seed)
    : m_height(seed, 50, 0.0f, 5, 0.4f),
      m_temperature(seed + 1, 50 * worldScale, 0.0f, 5, 0.4f),
      m_concentration(seed + 2, 40 * worldScale, 0.0f, 5, 0.4f),
      m_altitude(seed + 3, 30 * worldScale, 0.0f, 5, 0.4f),
      m_blockRegistry(RegistryManager::Blocks()) {}

BlockID BiomeGenerator::voxelAt(const glm::ivec3& pos) {
    Biome* biome = biomeAt(pos.x, pos.z);
    if (!biome)
        return Block::AirID;
    return biome->getBlockAt(pos, height(pos.x, pos.z));
}

int BiomeGenerator::height(int x, int z) const {
    float baseHeight = m_height.get2D(x, z);

    if (!m_enableBlending) {
        Biome* biome = biomeAt(x, z);
        if (biome)
            return static_cast<int>(biome->getHeightModifier(x, z, baseHeight));
        return static_cast<int>(baseHeight * 8.0f) + 5;
    }

    auto blendData = getBiomeBlend(x, z);

    if (blendData.empty())
        return static_cast<int>(baseHeight * 8.0f) + 5;

    if (blendData.size() == 1)
        return static_cast<int>(blendData[0].biome->getHeightModifier(x, z, baseHeight));

    float blendedHeight = 0.0f;
    for (const auto& data : blendData)
        blendedHeight += data.biome->getHeightModifier(x, z, baseHeight) * data.weight;

    return static_cast<int>(blendedHeight);
}

void BiomeGenerator::populate(Chunk& chunk) {
    paintColumns(chunk);
}

void BiomeGenerator::paintColumns(Chunk& chunk) {
    IChunkData* chunkData = chunk.data();
    glm::ivec3 chunkPos = chunk.id() * chunk.dims();

    for (int x = 0; x < chunk.dims().x; x++) {
        for (int z = 0; z < chunk.dims().z; z++) {
            int worldX = x + chunkPos.x;
            int worldZ = z + chunkPos.z;

            Biome* biome = biomeAt(worldX, worldZ);
            if (!biome)
                continue;

            int h = height(worldX, worldZ);

            for (int y = 0; y < chunk.dims().y; y++) {
                glm::ivec3 worldPos = glm::ivec3(worldX, y + chunkPos.y, worldZ);
                BlockID blockID = biome->getBlockAt(worldPos, h);
                if (blockID != Block::AirID)
                    chunkData->setBlock({x, y, z}, blockID);
            }
        }
    }
}

Biome* BiomeGenerator::biomeAt(int x, int z) const {
    if (m_biomes.empty())
        return nullptr;

    float t = m_temperature.get2D(x, z);
    float c = m_concentration.get2D(x, z);
    float a = m_altitude.get2D(x, z);

    Biome* bestBiome = nullptr;
    float bestScore = std::numeric_limits<float>::max();
    int bestPriority = 0;

    for (const auto& biome : m_biomes) {
        switch (biome->altitude()) {
            case ParameterRange::Low:
                if (a > m_lowAltitude.max)
                    continue;
                break;
            case ParameterRange::Mid:
                if (a < m_lowAltitude.max || a > m_highAltitude.min)
                    continue;
                break;
            case ParameterRange::High:
                if (a < m_highAltitude.min)
                    continue;
                break;
        }

        float score = biome->fitScore(t, c);
        if (score < bestScore || (score == bestScore && biome->priority() > bestPriority)) {
            bestBiome = biome.get();
            bestScore = score;
            bestPriority = biome->priority();
        }
    }

    return bestBiome;
}

std::vector<BiomeBlendData> BiomeGenerator::getBiomeBlend(int x, int z) const {
    if (m_biomes.empty())
        return {};

    Biome* currentBiome = biomeAt(x, z);
    std::unordered_map<Biome*, int> biomeWeights;
    int totalSamples = 0;
    int diffSamples = 0;

    const int blendRadiusSq = m_blendRadius * m_blendRadius;
    for (int i = -m_blendRadius; i <= m_blendRadius; i++) {
        for (int j = -m_blendRadius; j <= m_blendRadius; j++) {
            if (glm::length2(glm::vec2(i, j)) > blendRadiusSq)
                continue;

            totalSamples++;

            Biome* biome = biomeAt(x + i, z + j);
            if (biome == currentBiome)
                continue;

            biomeWeights[biome]++;
            diffSamples++;
        }
    }

    if (diffSamples == 0)
        return {{currentBiome, 1.0f}};

    std::vector<BiomeBlendData> blendData;
    float centerWeight = (totalSamples - diffSamples) / static_cast<float>(totalSamples);
    blendData.push_back({currentBiome, centerWeight});

    for (const auto& [biome, weight] : biomeWeights) {
        blendData.push_back({biome, static_cast<float>(weight) / static_cast<float>(totalSamples)});
    }

    return blendData;
}

void BiomeGenerator::add(std::unique_ptr<Biome>&& biome) {
    m_biomes.push_back(std::move(biome));
}

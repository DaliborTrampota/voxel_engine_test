#pragma once


#include <Globals.h>
#include <level/ITerrainGenerator.h>
#include <random/PerlinNoise.h>

#include <block/Block.h>
#include <data/RegistryManager.h>
#include <level/Chunk.h>
#include <level/data/IChunkData.h>
#include <random/Random.h>
#include "glm/ext/vector_int3.hpp"

class TerrainGenerator : public engine::ITerrainGenerator {
  public:
    TerrainGenerator(engine::PerlinNoise::Seed seed, float scale, int octaves)
        : m_noise(seed, scale, 0.0f, octaves) {}

    engine::BlockID voxelAt(const glm::ivec3& pos) override final {
        int y = height(pos.x, pos.z);
        if (pos.y == y) {
            return engine::RegistryManager::Blocks().get("faktSuperBlockBracho")->getID();
        }
        if (pos.y > y)
            return engine::Block::AirID;
        if (pos.y > y - 3)
            return engine::RegistryManager::Blocks().get("ground")->getID();
        return engine::RegistryManager::Blocks().get("underground")->getID();
    }


    int height(int x, int z) const override final { return m_noise.get2D(x, z) * m_heightScale; }

    void populate(engine::Chunk& chunk) override final {
        engine::IChunkData* data = chunk.data();
        glm::ivec3 chunkDims = chunk.dims();
        auto chunkCoords = chunk.id() * chunkDims;
        for (int x = 0; x < chunkDims.x; ++x) {
            for (int y = 0; y < chunkDims.y; ++y) {
                for (int z = 0; z < chunkDims.z; ++z) {
                    engine::BlockID blockID = voxelAt(glm::ivec3(x, y, z) + chunkCoords);
                    glm::ivec3 globalCoords = chunk.id() * chunk.dims() + glm::ivec3(x, y, z);
                    if (globalCoords.y == height(globalCoords.x, globalCoords.z)) {
                        int val =
                            engine::Random::randomRange2D(globalCoords.x, globalCoords.z, 0, 100);
                        if (val > 90) {
                            if (y + 1 < 16) {
                                data->setBlock(
                                    glm::ivec3(x, y + 1, z),
                                    engine::RegistryManager::Blocks().get("underground")->getID()
                                );
                            }
                        }
                        if (val < 5) {
                            for (int i = y + 1; i < std::min(16, y + 4); ++i) {
                                data->setBlock(
                                    glm::ivec3(x, i, z),
                                    engine::RegistryManager::Blocks().get("underground")->getID()
                                );
                            }
                        }
                    }
                    if (blockID != engine::Block::AirID) {
                        data->setBlock(glm::ivec3(x, y, z), blockID);
                    }
                }
            }
        }
    }


  private:
    engine::PerlinNoise m_noise;
    float m_heightScale = 5.0f;
};
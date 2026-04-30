#pragma once


#include <level/ITerrainGenerator.h>
#include <random/PerlinNoise.h>

#include <block/Block.h>
#include <data/RegistryManager.h>
#include <level/Chunk.h>
#include <level/data/IChunkData.h>


class TerrainGenerator : public engine::ITerrainGenerator {
  public:
    TerrainGenerator(engine::PerlinNoise::Seed seed, float scale, int octaves)
        : m_noise(seed, scale, 0.0f, octaves) {}

    engine::BlockID voxelAt(const glm::ivec3& pos) override final {
        int y = height(pos.x, pos.z);

        if (pos.y == y)
            return engine::RegistryManager::Blocks().get("special")->getID();
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
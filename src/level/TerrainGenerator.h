#pragma once


#include <level/ITerrainGenerator.h>
#include <level/World.h>
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
        if (pos.y > y)
            return engine::Block::AirID;
        if (pos.y == y) {
            return engine::RegistryManager::Blocks().get("newblock")->getID();
        }
        if (pos.y > y - 3)
            return engine::RegistryManager::Blocks().get("ground")->getID();
        return engine::RegistryManager::Blocks().get("underground")->getID();
    }


    int height(int x, int z) const override final { return m_noise.get2D(x, z) * m_heightScale; }

    glm::ivec3 worldPos(engine::Chunk& chunk, glm::ivec3 localPos) {
        return chunk.id() * chunk.dims() + localPos;
    }

    void tryAddBlock(engine::IChunkData* data, glm::ivec3 pos, engine::BlockID blockID) {
        if (pos.x < 0 || pos.x >= data->dims.x) {
            return;
        }
        if (pos.y < 0 || pos.y >= data->dims.y) {
            return;
        }
        if (pos.z < 0 || pos.z >= data->dims.z) {
            return;
        }

        data->setBlock(pos, blockID);
    }

    void populateSurface(engine::Chunk& chunk) {
        auto kmen = engine::RegistryManager::Blocks().get("ground")->getID();
        auto kamen = engine::RegistryManager::Blocks().get("underground")->getID();
        auto chunkPos = chunk.id() * chunk.dims();

        engine::IChunkData* data = chunk.data();
        glm::ivec3 chunkDims = chunk.dims();
        for (int x = 0; x < chunkDims.x; ++x) {
            for (int z = 0; z < chunkDims.z; ++z) {
                int y = height(chunkPos.x + x, chunkPos.z + z) - chunkPos.y;  // local y

                // if y not in chunk
                if (y < 0 || y >= chunkDims.y) {
                    continue;
                }


                // 5%
                if (engine::Random::random2D(chunkPos.x + x, chunkPos.z + z) < 0.05f) {
                    tryAddBlock(data, glm::ivec3(x, y + 1, z), kmen);
                    tryAddBlock(data, glm::ivec3(x, y + 2, z), kmen);
                    tryAddBlock(data, glm::ivec3(x, y + 3, z), kmen);
                }

                else if (engine::Random::random2D(chunkPos.x + x, chunkPos.z + z) < 0.1f) {
                    tryAddBlock(data, glm::ivec3(x, y + 1, z), kamen);
                }
            }
        }
    }

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

        populateSurface(chunk);
    }


  private:
    engine::PerlinNoise m_noise;
    float m_heightScale = 5.0f;
};
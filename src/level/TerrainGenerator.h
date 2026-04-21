#pragma once


#include <level/ITerrainGenerator.h>
#include <random/PerlinNoise.h>

#include <block/Block.h>
#include <random/Random.h>
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
        if (pos.y == y)
            return engine::RegistryManager::Blocks().get("surface")->getID();
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
                        // Snad je to ok :D
                        glm::ivec3 position(x, y, z);
                        if (position.y = height(position.x, position.z)) {
                            bool spawnTree = engine::Random::randomRange3D(x, y, z, 0, 100) < 5;
                            bool spawnRock = engine::Random::randomRange3D(x, y, z, 0, 100) < 15;
                            int startY = y + 1;

                            int n = chunk.dims().y;
                            if (spawnTree && y < (n - 3)) {
                                int treeID =
                                    engine::RegistryManager::Blocks().get("ground")->getID();
                                for (int locY = startY; locY < startY + 3; ++locY) {
                                    data->setBlock(glm::ivec3(x, locY, z), treeID);
                                }
                                continue;
                            }
                            if (spawnRock && y < (n - 1)) {
                                int rockID =
                                    engine::RegistryManager::Blocks().get("underground")->getID();
                                data->setBlock(glm::ivec3(x, startY, z), rockID);
                            }

                        }
                    }
                }
            }
        }
    }


  private:
    engine::PerlinNoise m_noise;
    float m_heightScale = 5.0f;
};
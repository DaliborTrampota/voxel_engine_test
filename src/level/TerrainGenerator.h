#pragma once


#include <level/ITerrainGenerator.h>
#include <random/PerlinNoise.h>

#include <block/Block.h>
#include <data/RegistryManager.h>
#include <level/Chunk.h>
#include <level/data/IChunkData.h>
#include <random/Random.h>


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
                        if (blockID == 11) {
                            //if (engine::Random::randomRange2D(x,z, 0, 100) <= 5) {
                            //    data->setBlock(glm::ivec3(x, y+1, z), 9);
                            //    data->setBlock(glm::ivec3(x, y+2, z), 9);
                            //    data->setBlock(glm::ivec3(x, y+3, z), 9);
                            //} 
                            //else if (engine ::Random::randomRange2D(x, z, 0, 100) <= 10) {
                            //    data->setBlock(glm::ivec3(x, y+1, z), 10);
                            //}
                            if (engine::Random::randomRange2D(x, z,0,100) <= 5 && y +3 < height(x,z)) {
                                data->setBlock(glm::ivec3(x, y+1, z), 9);
                                data->setBlock(glm::ivec3(x, y+2, z), 9);
                                data->setBlock(glm::ivec3(x, y+3, z), 9);
                            } else if (engine ::Random::randomRange2D(x, z, 0,100) <= 10 && y+1 < height(x, z)) {
                                data->setBlock(glm::ivec3(x, y+1, z), 10);
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
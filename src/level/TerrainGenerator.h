#pragma once


#include <level/ITerrainGenerator.h>
#include <random/PerlinNoise.h>
#include <random/Random.h>
#include <iostream>

#include <block/Block.h>
#include <data/RegistryManager.h>
#include <level/Chunk.h>
#include <level/data/IChunkData.h>


class TerrainGenerator : public engine::ITerrainGenerator {
  public:
    TerrainGenerator(engine::PerlinNoise::Seed seed, float scale, int octaves)
        : m_noise(seed, scale, 0.0f, octaves) {}

    static constexpr engine::BlockID GroundID = 9;
    static constexpr engine::BlockID UGroundID = 10;

    engine::BlockID voxelAt(const glm::ivec3& pos) override final {
        int y = height(pos.x, pos.z);
        if (pos.y > y)
            return engine::Block::AirID;
        if (pos.y == y){
            return engine::RegistryManager::Blocks().get("grassground")->getID();
        }
        if (pos.y > y - 3)
            return engine::RegistryManager::Blocks().get("ground")->getID();
        return engine::RegistryManager::Blocks().get("underground")->getID();
    }


    int height(int x, int z) const override final { return m_noise.get2D(x, z) * m_heightScale; }

    void spawnRock(int x, int y, int z, engine::Chunk& chunk){
        
        // std::cout<<"rock spawned"<<std::endl;
        engine::IChunkData* data = chunk.data();
        data->setBlock(glm::ivec3(x, y, z), UGroundID);

    }
    
    void spawnTree(int x, int y, int z, engine::Chunk& chunk){
        
        // std::cout<<"rock spawned"<<std::endl;
        engine::IChunkData* data = chunk.data();
        data->setBlock(glm::ivec3(x, y, z),   GroundID);
        data->setBlock(glm::ivec3(x, y+1, z), GroundID);
        data->setBlock(glm::ivec3(x, y+2, z), GroundID);

    }
    
    void populate(engine::Chunk& chunk) override final {
        engine::IChunkData* data = chunk.data();
        glm::ivec3 chunkDims = chunk.dims();
        
        engine::BlockID blockID;
        engine::BlockID prev;
        auto chunkCoords = chunk.id() * chunkDims;
        for (int x = 0; x < chunkDims.x; ++x) {
            for (int z = 0; z < chunkDims.z; ++z) {
                prev=engine::Block::AirID;
                for (int y = 0; y < chunkDims.y; ++y) {
                    blockID = voxelAt(glm::ivec3(x, y, z) + chunkCoords);
                    if (blockID != engine::Block::AirID) {
                        data->setBlock(glm::ivec3(x, y, z), blockID);
                    }
                    else{
                        if(prev != engine::Block::AirID){
                            switch(engine::Random::randomRange2D(x+chunkCoords.x, z+chunkCoords.z, 0 , 20)){
                                case 0: case 1:
                                    spawnRock(x,y,z,chunk);
                                    break;
                                case 2:
                                    spawnTree(x,y,z,chunk);
                                    break;
                            }
                        }
                    }
                    prev=blockID;
                    
                }
            }
        }
    }


  private:
    engine::PerlinNoise m_noise;
    float m_heightScale = 5.0f;
};

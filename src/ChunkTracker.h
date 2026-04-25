#pragma once
#include <level/events/LevelEventSite.h>
#include <level/events/LevelEvents.h>
#include <iostream>
#include <string>

class ChunkTracker : 
        public engine::LevelEventSite 
{
  protected:
    void chunkBeforeLoadEvent(engine::ChunkBeforeLoadEvent* e) override {
        // e->chunk is the Chunk about to be populated.
        // Set e->chunk->data()->populated = true here to skip the terrain generator
        // (useful when loading a saved chunk from disk).
        engine::ChunkID id = e->chunk->id();
        std::string chunkCoord =
            "(" + std::to_string(id.x) + ", " + std::to_string(id.y) + ", " + std::to_string(id.z) + ")";
        std::cout << " Chunk loaded: " << chunkCoord
                  << " | Total loaded so far : " << totalChunksLoaded << std::endl;
        totalChunksLoaded++;
    }
    void chunkUnloadEvent(engine::ChunkUnloadEvent* e) override {
        // e->chunk is the chunk being unloaded.
    }
  private:
    size_t totalChunksLoaded = 0;
};

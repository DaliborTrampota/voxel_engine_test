#pragma once
#include <iostream>
#include <level/events/LevelEventSite.h>
#include <level/events/LevelEvents.h>
#include <level/Chunk.h>

class ChunkTracer : public engine::LevelEventSite {
  public:
    ChunkTracer() : loadedCounter(0) {}

  protected:
    void chunkBeforeLoadEvent(engine::ChunkBeforeLoadEvent* pEvent) override {
        std::cout << "Chunk loaded: (" << pEvent->chunk->id().x << "," 
                  << pEvent->chunk->id().y
                  << "," << pEvent->chunk->id().z
                  << ") | Total loaded so far : " << loadedCounter << std::endl;
        loadedCounter++;
    };

    int loadedCounter;
};

#include <iostream>
#include "level/Chunk.h"
#include "level/events/LevelEventSite.h"
#include "level/events/LevelEvents.h"

class ChunkTracker : public engine::LevelEventSite {
  public:
    ChunkTracker() {}


    void chunkBeforeLoadEvent(engine::ChunkBeforeLoadEvent* e) override {
        std::cout << "Chunk loaded: " << e->chunk.get()->id();
        std::cout << " total loaded so far: " << ++m_Loaded << "\n";
    }

  private:
    int m_Loaded = 0;
};
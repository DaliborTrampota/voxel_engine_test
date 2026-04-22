#pragma once


#include <level/ITerrainGenerator.h>
#include <level/World.h>
#include <level/events/LevelEventSite.h>
#include <level/events/LevelEvents.h>
#include <random/PerlinNoise.h>

#include <block/Block.h>
#include <data/RegistryManager.h>
#include <level/Chunk.h>
#include <level/data/IChunkData.h>

#include <cstdio>


class ChunkTracker : public engine::LevelEventSite {
  public:
    ChunkTracker() { total = 0; }

    int total;

    void chunkBeforeLoadEvent(engine::ChunkBeforeLoadEvent* e) override {
        // e->chunk is the Chunk about to be populated.
        // Set e->chunk->data()->populated = true here to skip the terrain generator
        // (useful when loading a saved chunk from disk).
        ChunkLoadedNotification(e->chunk->id());
    }

    void ChunkLoadedNotification(glm::ivec3 pos) {
        printf(
            "Chunk loaded: (%d, %d, %d)  |  Total loaded so far: %d\n", pos.x, pos.y, pos.z, ++total
        );
    }
};

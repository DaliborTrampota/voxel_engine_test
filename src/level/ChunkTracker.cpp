#include "ChunkTracker.h"

#include "level/Chunk.h"
#include "level/events/LevelEvents.h"

void ChunkTracker::chunkBeforeLoadEvent(engine::ChunkBeforeLoadEvent* pEvent) {
    auto chunkPos = pEvent->chunk->position();

    std::cout <<
        "Chunk loaded: (" << chunkPos.x << ", " << chunkPos.y << ", " << chunkPos.z << ")" <<
        "  |  Total loaded so far: " << ++m_total << std::endl;
}
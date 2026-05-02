#pragma once

#include <level/events/LevelEventSite.h>

class ChunkTracker : public engine::LevelEventSite {
public:
    ChunkTracker(): m_total(0) {}

  protected:
    void chunkBeforeLoadEvent(engine::ChunkBeforeLoadEvent* pEvent) override;

private:
    int m_total;
};


#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>
#include <random/Random.h>
#include <cmath>
#include <glm/glm.hpp>

class FracturedBasaltFieldsBiome : public Biome {
  public:
    FracturedBasaltFieldsBiome()
        : Biome("FracturedBasaltFields", {-1.0f, 1.0f}, {-1.0f, 0.1f}, ParameterRange::Low, 10) {
        auto& registry = engine::RegistryManager::Blocks();

        m_descriptor.heightScale = 1.0f;
        m_descriptor.heightBase = 62.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("basalt")->getID()}, {1}}, .depth = -1}
        );
    }

    float getHeightModifier(int x, int z, float baseHeight) const override {
        return Biome::getHeightModifier(x, z, baseHeight) - getFractureDepth(x, z);
    }

  private:
    // Voronoi cracked-clay: each block belongs to its nearest jittered cell point.
    // The fracture is the locus where two cells are equidistant (F2 - F1 ≈ 0),
    // giving polygonal flat tiles with sharp narrow cracks between them.
    static constexpr int m_cellSize = 12;           // ≈ tile diameter in blocks
    static constexpr float m_jitter = 0.85f;        // 0 = perfect grid, 1 = max randomness
    static constexpr float m_fractureWidth = 3.0f;  // crack visible width in blocks
    static constexpr float m_maxDepth = 6.0f;       // crack depth at the seam
    static constexpr float m_wallSharpness = 4.0f;  // higher = sharper V-walls

    static int floorDiv(int a, int b) { return (a >= 0) ? (a / b) : ((a - b + 1) / b); }

    float getFractureDepth(int x, int z) const {
        int cellX = floorDiv(x, m_cellSize);
        int cellZ = floorDiv(z, m_cellSize);

        float d1Sq = std::numeric_limits<float>::max();
        float d2Sq = std::numeric_limits<float>::max();

        for (int dx = -1; dx <= 1; dx++) {
            for (int dz = -1; dz <= 1; dz++) {
                int cx = cellX + dx;
                int cz = cellZ + dz;

                float jx = engine::Random::random2D(cx, cz);
                float jz = engine::Random::random2D(cx + 7919, cz + 6917);

                float px = (cx + 0.5f + (jx - 0.5f) * m_jitter) * m_cellSize;
                float pz = (cz + 0.5f + (jz - 0.5f) * m_jitter) * m_cellSize;

                float ddx = static_cast<float>(x) - px;
                float ddz = static_cast<float>(z) - pz;
                float distSq = ddx * ddx + ddz * ddz;

                if (distSq < d1Sq) {
                    d2Sq = d1Sq;
                    d1Sq = distSq;
                } else if (distSq < d2Sq) {
                    d2Sq = distSq;
                }
            }
        }

        // Edge distance: how far this block sits from the equidistant boundary
        // between its two closest cells. Zero on the seam, grows inside the tile.
        float edge = std::sqrt(d2Sq) - std::sqrt(d1Sq);
        if (edge >= m_fractureWidth)
            return 0.0f;

        float frac = 1.0f - edge / m_fractureWidth;
        return std::pow(frac, m_wallSharpness) * m_maxDepth;
    }
};
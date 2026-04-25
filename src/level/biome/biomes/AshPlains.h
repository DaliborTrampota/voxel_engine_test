#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>
#include <random/Random.h>
#include <cmath>
#include <glm/glm.hpp>


class AshPlainsBiome : public Biome {
  public:
    AshPlainsBiome() : Biome("AshPlains", {-1.0f, 1.0f}, {-1.0f, 1.0f}, ParameterRange::Low) {
        auto& registry = engine::RegistryManager::Blocks();

        m_descriptor.heightScale = 8.0f;
        m_descriptor.heightBase = 62.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("ash")->getID()}, {1}}, .depth = 8}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("compressed_ash")->getID()}, {1}}, .depth = 30}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("slate")->getID()}, {1}}, .depth = -1}
        );
    }

    float getHeightModifier(int x, int z, float baseHeight) const override {
        float base = Biome::getHeightModifier(x, z, baseHeight);

        // Point-based sinkhole generation for circular, scattered features
        float sinkholeDepth = getSinkholeDepth(x, z);

        return base - sinkholeDepth;
    }

  private:
    // Sinkhole parameters
    static constexpr int m_cellSize = 64;          // Grid cell size (larger = more spread out)
    static constexpr float m_spawnChance = 0.15f;  // Probability of sinkhole per cell (15% = rare)
    static constexpr float m_maxRadius = 20.0f;    // Maximum radius of sinkhole
    static constexpr float m_maxDepth = 50.0f;     // Maximum depth of sinkhole (deeper)
    static constexpr float m_falloffPower =
        6.0f;  // Higher = sharper edges (1.0 = linear, 2.0 = quadratic, 4.0+ = sharp)
    static constexpr float m_flatBottomRadius =
        0.4f;  // Fraction of radius that's flat (0.4 = 40% of radius)
    static constexpr float m_roughnessStrength =
        8.0f;  // How rough the bottom is (higher = rougher)
    static constexpr float m_roughnessScale =
        0.5f;  // Scale of roughness noise (lower = larger features)

    /**
     * Calculate sinkhole depth at position (x, z) using point-based generation.
     * Divides world into cells, randomly places sinkhole centers, then applies
     * circular falloff based on distance to nearest center.
     */
    float getSinkholeDepth(int x, int z) const {
        // TODO floor division builtin or write helper
        int cellX = (x < 0) ? ((x + 1) / m_cellSize - 1) : (x / m_cellSize);
        int cellZ = (z < 0) ? ((z + 1) / m_cellSize - 1) : (z / m_cellSize);

        float minDistance = m_maxRadius + 1.0f;  // Start beyond max radius

        // Check current cell and 8 neighboring cells for sinkhole centers
        for (int dx = -1; dx <= 1; dx++) {
            for (int dz = -1; dz <= 1; dz++) {
                int checkCellX = cellX + dx;
                int checkCellZ = cellZ + dz;

                // Use deterministic random to check if this cell has a sinkhole
                float spawnRoll = engine::Random::random2D(checkCellX, checkCellZ);
                if (spawnRoll > m_spawnChance) {
                    continue;  // No sinkhole in this cell
                }

                // Get sinkhole center position within this cell (deterministic)
                float centerX =
                    static_cast<float>(checkCellX * m_cellSize) +
                    engine::Random::random2D(checkCellX, checkCellZ + 1000) * m_cellSize;
                float centerZ =
                    static_cast<float>(checkCellZ * m_cellSize) +
                    engine::Random::random2D(checkCellX + 1000, checkCellZ) * m_cellSize;

                // Calculate distance from current position to sinkhole center
                float dx2 = static_cast<float>(x) - centerX;
                float dz2 = static_cast<float>(z) - centerZ;
                float distance = std::sqrt(dx2 * dx2 + dz2 * dz2);

                // Track minimum distance to any sinkhole
                if (distance < minDistance) {
                    minDistance = distance;
                }
            }
        }

        // If we're within sinkhole radius, calculate depth
        if (minDistance < m_maxRadius) {
            // Normalize distance to [0, 1] where 0 = center, 1 = edge
            float normalizedDist = minDistance / m_maxRadius;

            // Create flatter bottom: constant depth in center, then falloff toward edges
            float baseDepth;
            if (normalizedDist < m_flatBottomRadius) {
                // Flat bottom area - constant depth
                baseDepth = m_maxDepth;
            } else {
                // Sloped sides - use power falloff
                float sideDist =
                    (normalizedDist - m_flatBottomRadius) / (1.0f - m_flatBottomRadius);
                float falloff = 1.0f - sideDist;
                float falloffResult = std::pow(falloff, m_falloffPower);
                baseDepth = falloffResult * m_maxDepth;
            }

            // Add roughness/noise to the bottom (stronger near center, fades toward edges)
            float roughnessFactor = 1.0f - normalizedDist;      // 1.0 at center, 0.0 at edge
            roughnessFactor = std::pow(roughnessFactor, 2.0f);  // Fade out faster

            // Use deterministic noise for roughness (different seed to avoid correlation)
            float noiseX = static_cast<float>(x) * m_roughnessScale;
            float noiseZ = static_cast<float>(z) * m_roughnessScale;
            float roughness = (engine::Random::random2D(
                                   static_cast<int>(noiseX * 1000.0f),
                                   static_cast<int>(noiseZ * 1000.0f)
                               ) -
                               0.5f) *
                              2.0f;  // Range: [-1, 1]

            float roughnessOffset = roughness * m_roughnessStrength * roughnessFactor;

            return baseDepth + roughnessOffset;
        }

        return 0.0f;  // No sinkhole at this position
    }

    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};
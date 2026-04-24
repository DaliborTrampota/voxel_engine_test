#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>
#include <random/Random.h>
#include <array>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

namespace {

    inline float distanceToLineSegment(
        const glm::vec2& pt, const glm::vec2& lineStart, const glm::vec2& lineEnd
    ) {
        glm::vec2 AB = lineEnd - lineStart;
        glm::vec2 AP = pt - lineStart;

        // If line segment has zero length, return distance to start point
        float abLengthSq = glm::dot(AB, AB);
        if (abLengthSq < 0.0001f) {
            return glm::length(AP);
        }

        float t = glm::dot(AP, AB) / abLengthSq;
        t = std::max(0.0f, std::min(1.0f, t));

        glm::vec2 closestPoint = lineStart + t * AB;
        return glm::length(pt - closestPoint);
    }

}  // namespace

class FracturedBasaltFieldsBiome : public Biome {
  public:
    FracturedBasaltFieldsBiome()
        : Biome("FracturedBasaltFields", {0.0f, 1.0f}, {-1.0f, 0.1f}, ParameterRange::Low, 10) {
        auto& registry = engine::RegistryManager::Blocks();

        m_descriptor.heightScale = 4.0f;
        m_descriptor.heightBase = 50.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("basalt")->getID()}, {1}}, .depth = -1}
        );
    }

    float getHeightModifier(int x, int z, float baseHeight) const {
        // voronoi diagram, where line there is a fracture?
        return Biome::getHeightModifier(x, z, baseHeight) - getFractureDepth(x, z);
    }

  private:
    int m_cellSize = 20;
    float m_fractureWidth = 0.75f;
    float m_fractureChance = 1.f;  //0.90f;
    int m_fractureDepth = 4;


    float getFractureDepth(int x, int z) const {
        // Use floor division for negative coordinates
        // For negative numbers, we need to subtract 1 before dividing
        int cellX = (x < 0) ? ((x + 1) / m_cellSize - 1) : (x / m_cellSize);
        int cellZ = (z < 0) ? ((z + 1) / m_cellSize - 1) : (z / m_cellSize);

        float aThis = engine::Random::random2D(cellX, cellZ);
        if (aThis > m_fractureChance)
            return 0.0f;

        struct CellPoint {
            glm::ivec2 cell;
            glm::vec2 position;
        };
        std::array<CellPoint, 4> points;
        glm::vec2 curCellPt = {0, 0};


        int idx = 0;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dz = -1; dz <= 1; dz++) {
                if (glm::abs(dz) + glm::abs(dx) > 1)
                    continue;

                int checkCellX = cellX + dx;
                int checkCellZ = cellZ + dz;

                // Find point in the cell but m_fractureWidth away from the cell border
                int centerX = checkCellX * m_cellSize + m_fractureWidth +
                              static_cast<int>(
                                  engine::Random::random2D(checkCellX, checkCellZ + 1000) *
                                  (m_cellSize - m_fractureWidth * 2)
                              );
                int centerZ = checkCellZ * m_cellSize + m_fractureWidth +
                              static_cast<int>(
                                  engine::Random::random2D(checkCellX + 1000, checkCellZ) *
                                  (m_cellSize - m_fractureWidth * 2)
                              );

                if (dx == 0 && dz == 0) {
                    curCellPt = {centerX, centerZ};
                    continue;
                }

                points[idx] = {.cell{checkCellX, checkCellZ}, .position{centerX, centerZ}};
                idx++;
            }
        }

        glm::vec2 currentPos(static_cast<float>(x), static_cast<float>(z));
        float minDistance = m_fractureWidth + 1.0f;  // Start beyond fracture width

        for (int i = 0; i < idx; i++) {
            const auto& pt = points[i];

            // Check if this neighbor cell has a fracture point
            float neighborChance = engine::Random::random2D(pt.cell.x, pt.cell.y);
            if (neighborChance > m_fractureChance)
                continue;


            float distance = distanceToLineSegment(currentPos, curCellPt, pt.position);

            if (distance < minDistance) {
                minDistance = distance;
            }
        }

        if (minDistance < m_fractureWidth) {
            // normalize 0 = center of fracture, 1 = edge
            float normalizedDist = minDistance / m_fractureWidth;

            // smooth falloff
            float depth = (1.0f - normalizedDist) * m_fractureDepth;
            return depth;
        }

        return 0.0f;
    }
};
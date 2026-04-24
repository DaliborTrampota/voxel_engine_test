#pragma once

#include <numeric>
#include <stdexcept>
#include <vector>


#include <block/Block.h>
#include <random/Random.h>

struct Palette {
    std::vector<engine::BlockID> blocks;
    std::vector<int> weights;
    int totalWeight;

    static Palette fromBlock(engine::BlockID block) { return Palette({block}, {1}); }

    // Constructor that calculates totalWeight automatically
    Palette() : totalWeight(0) {}
    Palette(std::vector<engine::BlockID> blocks_, std::vector<int> weights_)
        : blocks(std::move(blocks_)),
          weights(std::move(weights_)),
          totalWeight(std::accumulate(weights.begin(), weights.end(), 0)) {
        if (blocks.size() != weights.size()) {
            throw std::invalid_argument("blocks and weights must have the same size");
        }
    }

    engine::BlockID get3D(const glm::ivec3& pos) const {
        if (blocks.size() == 1) {
            return blocks[0];
        }
        float a = engine::Random::random3D(pos.x, pos.y, pos.z) * totalWeight;
        return get(a);
    }

    engine::BlockID get() const {
        if (blocks.size() == 1) {
            return blocks[0];
        }
        float a = engine::Random::random<float>() * totalWeight;
        return get(a);
    }

    bool operator==(const Palette& other) const {
        return blocks == other.blocks && weights == other.weights &&
               totalWeight == other.totalWeight;
    }

  private:
    engine::BlockID get(float a) const {
        int cursor = 0;
        for (int i = 0; i < weights.size(); i++) {
            cursor += weights[i];
            if (a < cursor) {
                return blocks[i];
            }
        }
        return blocks.back();  // Not possible
    }
};
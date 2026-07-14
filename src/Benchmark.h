#pragma once

#include <future>
#include <limits>
#include <memory>
#include <random>
#include <vector>

#include <glm/glm.hpp>
#include <level/ChunkID.h>
#include <scene/Updateable.h>


namespace engine {
    class World;
    class ITerrainGenerator;
}  // namespace engine

class Player;

class Benchmark : public engine::Updateable {
  public:
    Benchmark(
        std::shared_ptr<Player> player,
        std::shared_ptr<engine::World> world,
        const engine::ITerrainGenerator* generator
    );

    void start(int viewDistance);
    bool isRunning() const { return m_state != State::Idle && m_state != State::Done; }

    void update(float dt) override;

  private:
    enum class State {
        Idle,
        WaitingForChunks,
        WarmingUp,
        Cooldown,
        Rotating,
        Done,
    };

    void beginIteration();
    void finishIteration();

    static constexpr int kIterations = 8;
    static constexpr int kViewDistanceY = 3;
    static constexpr float kRotationDuration = 10.0f;
    static constexpr float kWarmupDuration = 5.0f;
    static constexpr float kCooldownDuration = 2.0f;
    static constexpr int kTeleportRangeChunks = 60;

    std::shared_ptr<Player> m_player;
    std::shared_ptr<engine::World> m_world;
    const engine::ITerrainGenerator* m_generator;

    State m_state = State::Idle;
    int m_iteration = 0;
    int m_viewDistance = 8;

    std::future<void> m_loadFuture;
    glm::ivec3 m_centerChunk{0};
    int m_groundHeight = 0;
    float m_startYaw = 0.0f;

    float m_warmupElapsed = 0.0f;
    float m_cooldownElapsed = 0.0f;
    float m_rotationElapsed = 0.0f;
    int m_frameCount = 0;
    float m_frameTimeAccum = 0.0f;
    float m_minDt = std::numeric_limits<float>::max();
    float m_maxDt = 0.0f;

    std::vector<float> m_iterationFps;
    std::vector<float> m_iterationMinFps;
    std::vector<float> m_iterationMaxFps;
    std::vector<engine::ChunkID> m_loadedSoFar;

    std::mt19937 m_rng;
};

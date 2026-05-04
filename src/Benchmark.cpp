#include "Benchmark.h"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <numeric>

#include <glm/gtc/constants.hpp>
#include <level/ITerrainGenerator.h>
#include <level/World.h>
#include <scene/Camera.h>

#include "player/Player.h"


Benchmark::Benchmark(
    std::shared_ptr<Player> player,
    std::shared_ptr<engine::World> world,
    const engine::ITerrainGenerator* generator
)
    : m_player(std::move(player)),
      m_world(std::move(world)),
      m_generator(generator),
      m_rng(std::random_device{}()) {}

void Benchmark::start(int viewDistance) {
    if (isRunning()) {
        printf("[Benchmark] already running\n");
        return;
    }

    m_viewDistance = viewDistance;
    m_iteration = 0;
    m_iterationFps.clear();
    m_loadedSoFar.assign(m_world->loadedChunks().begin(), m_world->loadedChunks().end());
    m_player->setInputFrozen(true);
    printf(
        "[Benchmark] starting: %d iterations, view distance %d chunks, %.1fs rotation\n",
        kIterations,
        m_viewDistance,
        kRotationDuration
    );
    beginIteration();
}

void Benchmark::beginIteration() {
    std::uniform_int_distribution<int> dist(-kTeleportRangeChunks, kTeleportRangeChunks);
    int cx = dist(m_rng);
    int cz = dist(m_rng);

    int worldX = cx * 16 + 8;
    int worldZ = cz * 16 + 8;
    m_groundHeight = m_generator->height(worldX, worldZ);
    int cy = m_groundHeight / 16;
    m_centerChunk = glm::ivec3(cx, cy, cz);

    glm::ivec3 viewDist(m_viewDistance, kViewDistanceY, m_viewDistance);
    glm::ivec3 from = m_centerChunk - viewDist;
    glm::ivec3 to = m_centerChunk + viewDist;

    auto inBox = [&](const engine::ChunkID& id) {
        return id.x >= from.x && id.x <= to.x && id.y >= from.y && id.y <= to.y &&
               id.z >= from.z && id.z <= to.z;
    };

    std::vector<engine::ChunkID> toUnload;
    toUnload.reserve(m_loadedSoFar.size());
    for (const auto& id : m_loadedSoFar) {
        if (!inBox(id))
            toUnload.push_back(id);
    }
    if (!toUnload.empty())
        m_world->unloadChunksFromMemory(toUnload);

    m_loadedSoFar.clear();
    m_loadedSoFar.reserve((to.x - from.x + 1) * (to.y - from.y + 1) * (to.z - from.z + 1));
    for (int x = from.x; x <= to.x; ++x)
        for (int y = from.y; y <= to.y; ++y)
            for (int z = from.z; z <= to.z; ++z)
                m_loadedSoFar.push_back(engine::ChunkID(x, y, z));

    m_loadFuture = m_world->loadChunks(from, to, true);

    m_state = State::WaitingForChunks;
    printf(
        "[Benchmark] iteration %d/%d: teleporting to chunk (%d, %d, %d), height %d\n",
        m_iteration + 1,
        kIterations,
        cx,
        cy,
        cz,
        m_groundHeight
    );
}

void Benchmark::finishIteration() {
    float elapsed = m_frameTimeAccum;
    float fps = elapsed > 0.0f ? static_cast<float>(m_frameCount) / elapsed : 0.0f;
    m_iterationFps.push_back(fps);
    printf(
        "[Benchmark] iteration %d/%d done: %d frames in %.2fs => %.2f fps\n",
        m_iteration + 1,
        kIterations,
        m_frameCount,
        elapsed,
        fps
    );

    m_iteration++;
    if (m_iteration >= kIterations) {
        float total = std::accumulate(m_iterationFps.begin(), m_iterationFps.end(), 0.0f);
        float avg = total / m_iterationFps.size();
        printf("[Benchmark] FINAL average across %d iterations: %.2f fps\n", kIterations, avg);
        if (!m_loadedSoFar.empty()) {
            m_world->unloadChunksFromMemory(m_loadedSoFar);
            m_loadedSoFar.clear();
        }
        m_state = State::Done;
        m_player->setInputFrozen(false);
        return;
    }
    beginIteration();
}

void Benchmark::update(float dt) {
    if (m_state == State::WaitingForChunks) {
        if (m_loadFuture.valid() &&
            m_loadFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            m_loadFuture.get();

            int worldX = m_centerChunk.x * 16 + 8;
            int worldZ = m_centerChunk.z * 16 + 8;
            glm::vec3 spawnPos(worldX, m_groundHeight + 2, worldZ);
            m_player->setPosition(spawnPos);

            m_startYaw = 0.0f;
            engine::Camera* cam = m_player->getCamera();
            cam->lookDirection(glm::vec3(std::cos(glm::radians(m_startYaw)), 0.0f,
                                         std::sin(glm::radians(m_startYaw))));

            m_rotationElapsed = 0.0f;
            m_frameCount = 0;
            m_frameTimeAccum = 0.0f;
            m_state = State::Rotating;
            printf("[Benchmark] chunks ready, starting rotation\n");
        }
        return;
    }

    if (m_state == State::Rotating) {
        m_rotationElapsed += dt;
        m_frameTimeAccum += dt;
        m_frameCount++;

        float t = std::min(m_rotationElapsed / kRotationDuration, 1.0f);
        float yawDeg = m_startYaw + 360.0f * t;
        float yawRad = glm::radians(yawDeg);

        engine::Camera* cam = m_player->getCamera();
        cam->lookDirection(glm::vec3(std::cos(yawRad), 0.0f, std::sin(yawRad)));

        if (m_rotationElapsed >= kRotationDuration) {
            finishIteration();
        }
    }
}

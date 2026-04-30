#pragma once

#include <GLFWUserPointer.h>
#include <render/Engine.h>


#include "player/Player.h"

#include "level/ChunkTracker.h"


namespace engine {
    class Camera;
    class Window;
}  // namespace engine

class Game : public engine::Engine {
  public:
    Game() = delete;
    Game(std::unique_ptr<engine::Window> window, glm::ivec2 dims);
    ~Game();

    void start();

    const std::weak_ptr<Player> getPlayer() const { return m_player; }

    void processInput();
    void render(double dt) override;

  private:
    engine::GLFWUserPointer m_pointer;

    std::shared_ptr<Player> m_player;
    engine::Camera* m_plrCamera;

    std::shared_ptr<engine::World> m_world;

    ChunkTracker m_chunkTracker;
};
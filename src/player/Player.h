#pragma once

#include <glm/glm.hpp>
#include <memory>

#include <level/Chunk.h>
#include <scene/Camera.h>
#include <scene/Updateable.h>
#include <utility/CoordUtils.h>
#include <input/events/GLFWEventSite.h>

namespace engine {
    class World;
}  // namespace engine

class Player : public engine::Updateable, public engine::GLFWEventSite {
  public:
    Player(engine::PerspectiveOptions opts);
    ~Player();

    void spawn(std::shared_ptr<engine::World> world, glm::vec3 spawnPos);

    void update(float dt) override;

    void move(glm::vec3 dir, float dt);
    void rotate(float dx, float dy, bool constrainPitch = true);
    void setPosition(glm::vec3 position);

    engine::Camera* getCamera() { return m_camera.get(); }
    const glm::vec3& position() const { return m_position; }

  protected:
    void mouseMoveEvent(engine::MouseEvent* pEvent) override{};
    void mouseButtonEvent(engine::MouseButtonEvent* pEvent) override { 
        engine::DDAResult res = engine::DDA(
            *m_world,
            m_camera->position(),
            m_camera->lookDirection(),
            5.0f,
            [](const engine::Block* block, const engine::Face*) -> bool {
                return block->getID() == engine::Block::AirID;
            }
        );
        m_world->setBlock(res.position, 0);

    };
    void keyboardEvent(engine::KeyboardEvent* pEvent) override {};

  private:
    glm::vec3 m_position;
    float m_speed = 10.f;
    float m_playerHeight = 1.8f;

    std::shared_ptr<engine::World> m_world;
    std::unique_ptr<engine::Camera> m_camera;

    glm::vec3 m_spawnpoint;
};

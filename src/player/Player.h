#pragma once

#include <glm/glm.hpp>
#include <memory>

#include <block/Block.h>
#include <input/events/GLFWEventSite.h>
#include <input/events/GLFWEvents.h>
#include <scene/Camera.h>
#include <scene/Updateable.h>


namespace engine {
    class World;
}  // namespace engine

class Player : public engine::Updateable,
               public engine::GLFWEventSite {
  public:
    Player(engine::PerspectiveOptions opts);
    ~Player();

    void spawn(std::shared_ptr<engine::World> world, glm::vec3 spawnPos);

    void update(float dt) override;
    void mouseButtonEvent(engine::MouseButtonEvent* pEvent) override;

    void move(glm::vec3 dir, float dt);
    void rotate(float dx, float dy, bool constrainPitch = true);
    void setPosition(glm::vec3 position);

    engine::Camera* getCamera() { return m_camera.get(); }
    const glm::vec3& position() const { return m_position; }

    void setHeldBlock(engine::BlockID id) { m_heldBlock = id; }
    engine::BlockID heldBlock() const { return m_heldBlock; }

  private:
    glm::vec3 m_position;
    float m_speed = 10.f;
    float m_playerHeight = 1.8f;
    engine::BlockID m_heldBlock = engine::Block::AirID;

    float m_fpsAccum = 0.0f;
    int m_fpsCount = 0;

    std::shared_ptr<engine::World> m_world;
    std::unique_ptr<engine::Camera> m_camera;

    glm::vec3 m_spawnpoint;
};

#pragma once

#include <glm/glm.hpp>
#include <memory>

#include <scene/Camera.h>
#include <level/Chunk.h>
#include <scene/Updateable.h>

namespace engine {
class World;
} // namespace engine

class Player : public engine::Updateable {
public:
  Player(engine::PerspectiveOptions opts);
  ~Player();

  void spawn(std::shared_ptr<engine::World> world, glm::vec3 spawnPos);

  void update(float dt) override;

  void move(glm::vec3 lDir, float dt);
  void rotate(float dx, float dy, bool constrainPitch = true);
  void setPosition(glm::vec3 position, float stepHeight = 0.0f);

  engine::Camera *getCamera() { return m_camera.get(); }
  const glm::vec3 &position() const { return m_position; }

private:
  glm::vec3 m_position;
  float m_speed = 10.f;
  float m_playerHeight = 1.8f;

  std::shared_ptr<engine::World> m_world;
  std::unique_ptr<engine::Camera> m_camera;

  glm::vec3 m_spawnpoint;
};

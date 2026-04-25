#include "Player.h"

#include "../GameServices.h"

#include <memory>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <block/Block.h>
#include <input/InputSystem.h>
#include <input/events/GLFWEvents.h>
#include <level/Chunk.h>
#include <level/World.h>
#include <utility/Algorithms.h>
#include <utility/CoordUtils.h>
#include <utility/Rotation.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

using namespace engine;

Player::Player(PerspectiveOptions opts) : m_world(nullptr), m_position(0, 0, 0) {
    m_camera = std::make_unique<Camera>(opts);
}

Player::~Player() {}

void Player::mouseButtonEvent(MouseButtonEvent* pEvent) {
    if (pEvent->action != GLFW_PRESS || !m_world)
        return;

    glm::vec3 pos = m_camera->position();
    glm::vec3 dir = m_camera->lookDirection();

    auto hit = engine::DDA(*m_world, pos, dir, 10.0f, [](const Block* b, const Face*) {
        return !b->isAir();
    });

    if (hit.block->isAir())
        return;

    if (pEvent->button == GLFW_MOUSE_BUTTON_LEFT) {
        m_world->setBlock(glm::ivec3(hit.position), Block::AirID);
    } else if (pEvent->button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (m_heldBlock == Block::AirID)
            return;
        glm::ivec3 placePos = glm::ivec3(hit.position) + glm::ivec3(hit.faceNormal);
        m_world->setBlock(placePos, m_heldBlock);
    }
}

void Player::spawn(std::shared_ptr<World> world, glm::vec3 spawnPos) {
    m_world = world;
    m_spawnpoint = spawnPos;
    setPosition(spawnPos);
    m_camera->lookAt(m_camera->position() + NORTH);

    printf("Spawning player at %.2f %.2f %.2f\n", spawnPos.x, spawnPos.y, spawnPos.z);
}

void Player::update(float dt) {
    auto input = GameServices::getInputSystem();
    float mouseX = input->getAxis(InputAxis::MouseX);
    float mouseY = input->getAxis(InputAxis::MouseY);

    rotate(mouseX, mouseY, true);

    float forward = input->getAxis(InputAxis::Forward);
    float sideways = input->getAxis(InputAxis::Sideways);

    bool up = input->isKey<Down>(GLFW_KEY_SPACE);
    bool down = input->isKey<Down>(GLFW_KEY_LEFT_SHIFT);
    float upDown = up - down;

    glm::vec3 moveDir(sideways, upDown, forward);
    if (!glm::all(glm::epsilonEqual(moveDir, glm::vec3{0.0f}, 0.01f))) {
        move(glm::normalize(moveDir), dt);
    }

    if (input->isKey<Pressed>(GLFW_KEY_P)) {
        spawn(m_world, m_spawnpoint);
    }

    if (input->isKey<Pressed>(GLFW_KEY_F)) {
        if (dt > 0.0f) {
            float fps = 1.0f / dt;
            m_fpsAccum += fps;
            m_fpsCount++;
            float avg = m_fpsAccum / m_fpsCount;
            printf("FPS: %.1f  |  avg: %.1f  |  frame: %.2f ms\n", fps, avg, dt * 1000.0f);
        }
    }

    if (input->isKey<Pressed>(GLFW_KEY_L)) {
        auto dir = m_camera->lookDirection();
        printf("Look direction: %.1f, %.1f, %.1f\n", dir.x, dir.y, dir.z);
        printf("Position: %.1f, %.1f, %.1f\n", m_position.x, m_position.y, m_position.z);
        printf(
            "Camera: %.1f, %.1f, %.1f\n",
            m_camera->position().x,
            m_camera->position().y,
            m_camera->position().z
        );
    }
}

void Player::move(glm::vec3 dir, float dt) {
    glm::quat rotation = m_camera->rotation(true);
    glm::vec3 moveStep = rotation * dir * m_speed;

    glm::vec3 displacement = moveStep * dt;
    setPosition(m_position + displacement);
}

void Player::rotate(float dx, float dy, bool constrainPitch) {
    m_camera->rotate(dx, dy, constrainPitch);
}

void Player::setPosition(glm::vec3 position) {
    m_position = position;
    position.y += m_playerHeight;

    m_camera->position(position);
}
#include "Game.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include <LWGL/render/ShaderProgram.h>

#include <GLFWUserPointer.h>
#include <audio/AudioManager.h>
#include <data/TextureLoader.h>
#include <data/TextureManager.h>
#include <input/InputSystem.h>
#include <level/World.h>
#include <render/Window.h>
#include <scene/Camera.h>
#include <scene/Sun.h>
#include <scene/Updateable.h>

#include <render/concrete/ScenePass.h>


#include "GameServices.h"
#include "level/TerrainGenerator.h"
#include "registration.h"

using namespace engine;

Game::Game(std::unique_ptr<Window> window, glm::ivec2 dims)
    : Engine(std::move(window)),
      m_plrCamera(nullptr) {
    m_window->makeCurrent();

    m_pointer = GLFWUserPointer{m_inputSystem.get(), m_window.get()};
    m_window->setUserPointer(&m_pointer);

    GameServices::setInputSystem(m_inputSystem.get());
    GameServices::setGame(this);
}

Game::~Game() {}

void Game::processInput() {
    if (m_inputSystem->isKey<Down>(GLFW_KEY_ESCAPE))
        window()->setShouldClose();
}

void Game::render(double dt) {
    processInput();
    fireUpdate(dt);

    // TODO: render world here?
    m_world->render(*this, m_plrCamera);
}

void Game::start() {
    // TODO: load textures?
    engine::TextureLoader::loadArray2D(engine::TextureManager::Get().blockTextures(), "resources/");
    // TODO: register geometries and blocks to registry?
    registerGeometries();
    registerBlocks();


    PerspectiveOptions opts;
    opts.fov = glm::radians(45.0f);
    opts.aspectRatio = m_window->aspectRatio();
    m_player = std::make_shared<Player>(opts);

    // TODO: spawn player to world?
    m_world = std::make_shared<engine::World>(std::make_unique<TerrainGenerator>(123, 50.0f, 5));
    m_world->loadChunks({-3, -3, -3}, {3, 3, 3});

    int height = m_world->getGenerator()->height(0, 0);
    glm::vec3 spawnPos(0, height, 0);
    m_player->spawn(m_world, spawnPos);

    // TODO: create/set directional light source?
    setDirectionalLightSource(
        std::make_shared<engine::Sun>(
            glm::ivec2(4096, 4096), m_plrCamera, glm::vec3(0.5f, -1.0f, 0.2f)
        )
    );

    subscribeUpdate(m_player);

    m_plrCamera = m_player->getCamera();
    m_plrCamera->lookAt(glm::vec3(0, 0, 0));
    m_window->subscribe(m_plrCamera);


    window()->mouseLock(true);
    gameloop();
}

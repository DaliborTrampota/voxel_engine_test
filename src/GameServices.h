#pragma once

#include <string>

class Game;

namespace engine {
    class InputSystem;
}  // namespace engine

class GameServices {
  public:
    static Game* getGame() { return m_game; }
    static void setGame(Game* game) { m_game = game; }

    static engine::InputSystem* getInputSystem() { return m_inputSystem; }
    static void setInputSystem(engine::InputSystem* inputSystem) { m_inputSystem = inputSystem; }

  private:
    static inline Game* m_game = nullptr;
    static inline engine::InputSystem* m_inputSystem = nullptr;
};
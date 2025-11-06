#include <iostream>
#include "game/GameBoard.h"
#include "graphics/Renderer.h"

class TetrisGame {
private:
    GameBoard board;
    Renderer renderer;
    bool gameRunning;
    double lastUpdateTime;

public:
    TetrisGame() : gameRunning(true), lastUpdateTime(0) {}

    bool initialize() {
        std::cout << "=== My Tetris Game ===" << std::endl;

        if (!renderer.initialize()) {
            std::cerr << "Failed to initialize renderer!" << std::endl;
            return false;
        }

        std::cout << "Game initialized successfully!" << std::endl;
        std::cout << "Controls: Arrow Keys or WASD to move" << std::endl;
        std::cout << "Q: Pause, Space: Fast Drop, ESC: Quit" << std::endl;
        return true;
    }

    void run() {
        double lastTime = glfwGetTime();

        while (gameRunning && !renderer.shouldClose()) {
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            renderer.processInput(board);

            // Обновление игры с переменным интервалом для плавности
            if (!board.isGamePaused() && !board.isAnimating()) {
                board.update(deltaTime);
            }
            else if (board.isAnimating()) {
                board.updateAnimation(deltaTime);
            }

            if (board.isGameOver()) {
                std::cout << "Game Over! Final Score: " << board.getScore() << std::endl;
                std::cout << "Time: " << board.getFormattedTime() << std::endl;
                gameRunning = false;
            }

            renderer.render(board);

            // Ограничение FPS
            double frameTime = glfwGetTime() - currentTime;
            if (frameTime < 1.0 / 60.0) {
                glfwWaitEventsTimeout(1.0 / 60.0 - frameTime);
            }
        }
    }

    void shutdown() {
        renderer.shutdown();
        std::cout << "Game shutdown complete." << std::endl;
    }
};

int main() {
    TetrisGame game;

    if (game.initialize()) {
        game.run();
    }

    game.shutdown();
    return 0;
}
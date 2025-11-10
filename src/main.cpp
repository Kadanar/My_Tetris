#include <iostream>
#include "game/GameBoard.h"
#include "graphics/Renderer.h"
#include "menu/MenuSystem.h"

class TetrisGame {
private:
    GameBoard board;
    Renderer renderer;
    MenuSystem menuSystem;
    bool gameRunning;
    bool gameInitialized;

public:
    TetrisGame() : gameRunning(true), gameInitialized(false) {}

    bool initialize() {
        std::cout << "=== My Tetris Game ===" << std::endl;

        if (!renderer.initialize()) {
            std::cerr << "Failed to initialize renderer!" << std::endl;
            return false;
        }

        std::cout << "Game initialized successfully!" << std::endl;
        return true;
    }

    void run() {
        double lastTime = glfwGetTime();

        while (gameRunning && !renderer.shouldClose()) {
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            // Handle different game states
            MenuState currentState = menuSystem.getState();

            if (currentState == MenuState::IN_GAME) {
                if (!gameInitialized) {
                    // Reset game when starting new game
                    board = GameBoard();
                    gameInitialized = true;
                }
                handleGameplay(deltaTime);
            }
            else {
                handleMenuState();
            }

            // Frame rate limiting
            double frameTime = glfwGetTime() - currentTime;
            if (frameTime < 1.0 / 60.0) {
                glfwWaitEventsTimeout(1.0 / 60.0 - frameTime);
            }
        }
    }

private:
    void handleGameplay(double deltaTime) {
        renderer.processInput(board);

        // Check for pause key
        if (glfwGetKey(renderer.getWindow(), GLFW_KEY_Q) == GLFW_PRESS) {
            menuSystem.setState(MenuState::PAUSE_MENU);
            return;
        }

        if (!board.isGamePaused()) {
            if (board.isAnimating()) {
                board.updateAnimation(deltaTime);
            }
            else {
                board.update(deltaTime);
            }
        }

        if (board.isGameOver()) {
            std::cout << "Game Over! Final Score: " << board.getScore() << std::endl;
            std::cout << "Time: " << board.getFormattedTime() << std::endl;
            menuSystem.setGameOverInfo(board.getScore(), board.getFormattedTime());
            menuSystem.setState(MenuState::GAME_OVER_MENU);
            gameInitialized = false;
        }

        renderer.render(board);
    }

    void handleMenuState() {
        MenuState currentState = menuSystem.getState();

        // Process menu input
        processMenuInput();

        // Update menu state
        menuSystem.update();

        // Render appropriate menu
        if (currentState == MenuState::GAME_OVER_MENU) {
            renderer.renderGameOverMenu(menuSystem);
        }
        else {
            renderer.renderMenu(menuSystem);
        }
    }

    void processMenuInput() {
        // Handle return from controls/highscreens screens
        if ((menuSystem.shouldShowControls() || menuSystem.shouldShowHighscores()) &&
            (glfwGetKey(renderer.getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS ||
                glfwGetKey(renderer.getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS ||
                glfwGetKey(renderer.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)) {
            menuSystem.setState(MenuState::MAIN_MENU);
            return;
        }

        // Handle menu navigation
        auto handleMenuKey = [&](int key) {
            if (glfwGetKey(renderer.getWindow(), key) == GLFW_PRESS) {
                menuSystem.handleKeyInput(key);
                // Small delay to prevent multiple triggers
                glfwWaitEventsTimeout(0.2);
            }
            };

        handleMenuKey(GLFW_KEY_W);
        handleMenuKey(GLFW_KEY_UP);
        handleMenuKey(GLFW_KEY_S);
        handleMenuKey(GLFW_KEY_DOWN);
        handleMenuKey(GLFW_KEY_ENTER);
        handleMenuKey(GLFW_KEY_SPACE);

        // Handle ESC for pause menu
        if (menuSystem.getState() == MenuState::PAUSE_MENU &&
            glfwGetKey(renderer.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            menuSystem.setState(MenuState::IN_GAME);
        }
    }

public:
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
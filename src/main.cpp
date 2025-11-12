#include <iostream>
#include "game/GameBoard.h"
#include "graphics/Renderer.h"
#include "menu/MenuSystem.h"
#include "db/Database.h"
#include <cstdlib>

class TetrisGame {
private:
    GameBoard board;
    Renderer renderer;
    MenuSystem menuSystem;
    bool gameRunning;
    bool gameInitialized;
    Database db;
    int currentPlayerId = -1;
    MenuState lastMenuState = MenuState::MAIN_MENU;

private:
    std::string getConnectionString() {
        const char* env = std::getenv("MY_TETRIS_CONN_STR");
        if (env && *env) return std::string(env);

        // Подключение к вашему SQL Server Express
        return std::string(
            "Driver={ODBC Driver 17 for SQL Server};"
            "Server=STH01-043-IT-LT\\SQLEXPRESS02;"
            "Database=MyTetris;"
            "Trusted_Connection=yes;"
            "Encrypt=No;"
        );
    }

public:
    TetrisGame() : gameRunning(true), gameInitialized(false) {}

    bool initialize() {
        std::cout << "=== My Tetris Game ===" << std::endl;

        if (!renderer.initialize()) {
            std::cerr << "Failed to initialize renderer!" << std::endl;
            return false;
        }

        // Connect to database
        std::string connStr = getConnectionString();
        std::cout << "Connecting to database..." << std::endl;

        if (!db.connect(connStr)) {
            std::cerr << "Database connect failed: " << db.getLastError() << std::endl;
            std::cout << "Continuing without database support..." << std::endl;
        }
        else {
            std::cout << "Database connected! Ensuring schema..." << std::endl;
            if (!db.ensureSchema()) {
                std::cerr << "Failed to ensure DB schema: " << db.getLastError() << std::endl;
                std::cout << "Continuing with limited database functionality..." << std::endl;
            }
            else {
                std::cout << "Database schema ready!" << std::endl;
            }
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
                    // Ensure player exists in DB when entering the game
                    if (db.isConnected()) {
                        auto pid = db.ensurePlayer(menuSystem.getCurrentPlayerName());
                        currentPlayerId = pid.has_value() ? *pid : -1;
                    }
                    // Reset game when starting new game
                    board = GameBoard();
                    gameInitialized = true;
                    board.setPaused(false);
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

            // Track state transitions for pause handling
            MenuState newState = menuSystem.getState();
            if (lastMenuState == MenuState::PAUSE_MENU && newState == MenuState::IN_GAME) {
                board.setPaused(false);
            }
            lastMenuState = newState;
        }
    }

private:
    void handleGameplay(double deltaTime) {
        renderer.processInput(board);

        // Check for pause key: Q opens pause menu and pauses the game
        if (glfwGetKey(renderer.getWindow(), GLFW_KEY_Q) == GLFW_PRESS) {
            board.setPaused(true);
            menuSystem.setState(MenuState::PAUSE_MENU);
            glfwWaitEventsTimeout(0.2);
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
            // Save score and stats
            if (db.isConnected() && currentPlayerId > 0) {
                int duration = static_cast<int>(board.getGameTime());
                db.insertScore(currentPlayerId, board.getScore(), board.getTotalClearedLines(), board.getLevel(), duration);
                db.insertGameStats(
                    currentPlayerId,
                    board.getScore(),
                    duration,
                    board.getLevel(),
                    board.getPieceCounts(),
                    board.getTotalClearedLines()
                );
            }
            gameInitialized = false;
        }

        renderer.render(board);
    }

    void handleMenuState() {
        MenuState currentState = menuSystem.getState();

        // Process menu input
        processMenuInput();

        // Fetch highscores when needed
        if (menuSystem.shouldShowHighscores() && db.isConnected()) {
            menuSystem.setHighscores(db.fetchTopScores(10));
        }

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

        // Handle name input character entry
        if (menuSystem.getState() == MenuState::NAME_INPUT) {
            // Letters A-Z
            for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; ++key) {
                if (glfwGetKey(renderer.getWindow(), key) == GLFW_PRESS) {
                    char c = static_cast<char>('A' + (key - GLFW_KEY_A));
                    menuSystem.handleCharInput(static_cast<unsigned int>(c));
                    glfwWaitEventsTimeout(0.1);
                }
            }
            // Digits 0-9
            for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; ++key) {
                if (glfwGetKey(renderer.getWindow(), key) == GLFW_PRESS) {
                    char c = static_cast<char>('0' + (key - GLFW_KEY_0));
                    menuSystem.handleCharInput(static_cast<unsigned int>(c));
                    glfwWaitEventsTimeout(0.1);
                }
            }
            // Space
            if (glfwGetKey(renderer.getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) {
                menuSystem.handleCharInput(static_cast<unsigned int>(' '));
                glfwWaitEventsTimeout(0.1);
            }
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
        handleMenuKey(GLFW_KEY_BACKSPACE);

        // Handle ESC for pause menu (resume)
        if (menuSystem.getState() == MenuState::PAUSE_MENU &&
            glfwGetKey(renderer.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            menuSystem.setState(MenuState::IN_GAME);
            board.setPaused(false);
            glfwWaitEventsTimeout(0.2);
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
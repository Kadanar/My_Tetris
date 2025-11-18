#include <iostream>
#include <Windows.h> // Добавьте для исправления кодировки
#include "game/GameBoard.h"
#include "graphics/Renderer.h"
#include "menu/MenuSystem.h"
#include "db/Database.h"
#include <cstdlib>

// Исправление кодировки консоли
class ConsoleSetup {
public:
    ConsoleSetup() {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    }
};

static ConsoleSetup consoleSetup;

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

        // Подключение к STH01-043-IT-LT\SQLEXPRESS02
        return std::string(
            "Driver={ODBC Driver 17 for SQL Server};"
            "Server=STH01-043-IT-LT\\SQLEXPRESS02;"
            "Database=MyTetris;"
            "Trusted_Connection=yes;"
            "Encrypt=yes;"
            "TrustServerCertificate=yes;"
        );
    }

public:
    TetrisGame() : gameRunning(true), gameInitialized(false) {}

    bool initialize() {
        std::cout << "=== My Tetris Game ===" << std::endl;
        std::cout << "Initializing renderer..." << std::endl;

        if (!renderer.initialize()) {
            std::cerr << "ERROR: Failed to initialize renderer!" << std::endl;
            return false;
        }

        std::string connStr = getConnectionString();
        std::cout << "Connecting to database..." << std::endl;
        std::cout << "Connection string: " << connStr << std::endl;

        // Тестируем подключение с детальной диагностикой
        std::cout << "Testing database connection..." << std::endl;
        if (!db.connect(connStr)) {
            std::cerr << "CRITICAL ERROR: Database connection failed!" << std::endl;
            std::cerr << "Error details: " << db.getLastError() << std::endl;
            std::cout << "GAME WILL RUN WITHOUT DATABASE SUPPORT" << std::endl;
            return true; // Продолжаем без БД
        }

        std::cout << "SUCCESS: Database connection established!" << std::endl;

        // Проверяем и создаем схему
        std::cout << "Creating database schema..." << std::endl;
        if (!db.ensureSchema()) {
            std::cerr << "SCHEMA ERROR: " << db.getLastError() << std::endl;
            std::cout << "DATABASE FUNCTIONALITY DISABLED" << std::endl;
            db.disconnect();
            return true;
        }

        std::cout << "Database schema ready!" << std::endl;
        std::cout << "Game initialized successfully!" << std::endl;
        return true;
    }

    void run() {
        double lastTime = glfwGetTime();

        while (gameRunning && !renderer.shouldClose()) {
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            MenuState currentState = menuSystem.getState();

            if (currentState == MenuState::IN_GAME) {
                if (!gameInitialized) {
                    std::cout << "\n--- STARTING NEW GAME ---" << std::endl;
                    std::cout << "Player name: " << menuSystem.getCurrentPlayerName() << std::endl;

                    // Регистрируем игрока в базе если подключение есть
                    if (db.isConnected()) {
                        std::cout << "Registering player in database..." << std::endl;
                        auto pid = db.ensurePlayer(menuSystem.getCurrentPlayerName());
                        if (pid.has_value()) {
                            currentPlayerId = *pid;
                            std::cout << "Player registered with ID: " << currentPlayerId << std::endl;
                        }
                        else {
                            std::cerr << "WARNING: Failed to register player in database!" << std::endl;
                            currentPlayerId = -1;
                        }
                    }
                    else {
                        std::cout << "Database not connected - skipping player registration" << std::endl;
                        currentPlayerId = -1;
                    }

                    board = GameBoard();
                    gameInitialized = true;
                    board.setPaused(false);
                    std::cout << "Game board initialized" << std::endl;
                }
                handleGameplay(deltaTime);
            }
            else {
                handleMenuState();
            }

            double frameTime = glfwGetTime() - currentTime;
            if (frameTime < 1.0 / 60.0) {
                glfwWaitEventsTimeout(1.0 / 60.0 - frameTime);
            }

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
            std::cout << "\n=== GAME OVER ===" << std::endl;
            std::cout << "Final score: " << board.getScore() << std::endl;
            std::cout << "Time: " << board.getFormattedTime() << std::endl;
            std::cout << "Level: " << board.getLevel() << std::endl;
            std::cout << "Lines: " << board.getTotalClearedLines() << std::endl;

            menuSystem.setGameOverInfo(board.getScore(), board.getFormattedTime());

            // Сохраняем результаты в базу данных с детальной диагностикой
            std::cout << "\n--- SAVING TO DATABASE ---" << std::endl;
            std::cout << "Database connected: " << (db.isConnected() ? "YES" : "NO") << std::endl;
            std::cout << "Player ID: " << currentPlayerId << std::endl;
            std::cout << "Player name: " << menuSystem.getCurrentPlayerName() << std::endl;

            if (db.isConnected() && currentPlayerId > 0) {
                int duration = static_cast<int>(board.getGameTime());
                std::cout << "Game duration: " << duration << " seconds" << std::endl;

                bool scoreSaved = db.insertScore(currentPlayerId, board.getScore(),
                    board.getTotalClearedLines(), board.getLevel(), duration);
                std::cout << "Score saved: " << (scoreSaved ? "SUCCESS" : "FAILED") << std::endl;

                bool statsSaved = db.insertGameStats(
                    currentPlayerId,
                    board.getScore(),
                    duration,
                    board.getLevel(),
                    board.getPieceCounts(),
                    board.getTotalClearedLines()
                );
                std::cout << "Stats saved: " << (statsSaved ? "SUCCESS" : "FAILED") << std::endl;

                if (scoreSaved && statsSaved) {
                    std::cout << "ALL DATA SAVED SUCCESSFULLY TO DATABASE!" << std::endl;
                }
                else {
                    std::cerr << "SOME DATA FAILED TO SAVE!" << std::endl;
                }
            }
            else {
                std::cout << "Cannot save to database - reasons:" << std::endl;
                if (!db.isConnected()) std::cout << " - Database not connected" << std::endl;
                if (currentPlayerId <= 0) std::cout << " - Invalid player ID" << std::endl;
            }

            menuSystem.setState(MenuState::GAME_OVER_MENU);
            gameInitialized = false;
        }

        renderer.render(board);
    }

    void handleMenuState() {
        MenuState currentState = menuSystem.getState();

        processMenuInput();

        if (menuSystem.shouldShowHighscores() && db.isConnected()) {
            std::cout << "Loading highscores from database..." << std::endl;
            auto highscores = db.fetchTopScores(10);
            std::cout << "Loaded " << highscores.size() << " highscores" << std::endl;
            menuSystem.setHighscores(highscores);
        }

        menuSystem.update();

        if (currentState == MenuState::GAME_OVER_MENU) {
            renderer.renderGameOverMenu(menuSystem);
        }
        else {
            renderer.renderMenu(menuSystem);
        }
    }

    void processMenuInput() {
        double currentTime = glfwGetTime();

        if ((menuSystem.getState() == MenuState::CONTROLS || menuSystem.getState() == MenuState::HIGHSCORES) &&
            (glfwGetKey(renderer.getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS ||
                glfwGetKey(renderer.getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS ||
                glfwGetKey(renderer.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)) {
            menuSystem.handleKeyInput(GLFW_KEY_ENTER, currentTime);
            return;
        }

        if (menuSystem.getState() == MenuState::NAME_INPUT) {
            if (glfwGetKey(renderer.getWindow(), GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
                menuSystem.handleKeyInput(GLFW_KEY_BACKSPACE, currentTime);
            }

            if (glfwGetKey(renderer.getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
                menuSystem.handleKeyInput(GLFW_KEY_ENTER, currentTime);
            }

            if (glfwGetKey(renderer.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                menuSystem.handleKeyInput(GLFW_KEY_ESCAPE, currentTime);
            }

            for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; ++key) {
                if (glfwGetKey(renderer.getWindow(), key) == GLFW_PRESS) {
                    char c = static_cast<char>('A' + (key - GLFW_KEY_A));
                    menuSystem.handleCharInput(static_cast<unsigned int>(c), currentTime);
                }
            }
            for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; ++key) {
                if (glfwGetKey(renderer.getWindow(), key) == GLFW_PRESS) {
                    char c = static_cast<char>('0' + (key - GLFW_KEY_0));
                    menuSystem.handleCharInput(static_cast<unsigned int>(c), currentTime);
                }
            }
            if (glfwGetKey(renderer.getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) {
                menuSystem.handleCharInput(static_cast<unsigned int>(' '), currentTime);
            }
        }

        if (menuSystem.getState() == MenuState::MAIN_MENU ||
            menuSystem.getState() == MenuState::PAUSE_MENU ||
            menuSystem.getState() == MenuState::GAME_OVER_MENU) {

            auto handleMenuKey = [&](int key) {
                if (glfwGetKey(renderer.getWindow(), key) == GLFW_PRESS) {
                    menuSystem.handleKeyInput(key, currentTime);
                }
                };

            handleMenuKey(GLFW_KEY_W);
            handleMenuKey(GLFW_KEY_UP);
            handleMenuKey(GLFW_KEY_S);
            handleMenuKey(GLFW_KEY_DOWN);
            handleMenuKey(GLFW_KEY_ENTER);
            handleMenuKey(GLFW_KEY_SPACE);
            handleMenuKey(GLFW_KEY_BACKSPACE);
        }

        if (menuSystem.getState() == MenuState::PAUSE_MENU &&
            glfwGetKey(renderer.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            menuSystem.setState(MenuState::IN_GAME);
            board.setPaused(false);
        }
    }

public:
    void shutdown() {
        renderer.shutdown();
        std::cout << "Game finished." << std::endl;
    }
};

int main() {
    TetrisGame game;

    std::cout << "==========================================" << std::endl;
    std::cout << "LAUNCHING TETRIS WITH DATABASE CONNECTION" << std::endl;
    std::cout << "==========================================" << std::endl;

    if (game.initialize()) {
        game.run();
    }
    else {
        std::cerr << "Failed to initialize game!" << std::endl;
    }

    game.shutdown();
    return 0;
}
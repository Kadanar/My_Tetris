#include "menu/MenuSystem.h"

// GLFW key definitions
#ifndef GLFW_KEY_W
#define GLFW_KEY_W 87
#define GLFW_KEY_S 83
#define GLFW_KEY_UP 265
#define GLFW_KEY_DOWN 264
#define GLFW_KEY_ENTER 257
#define GLFW_KEY_SPACE 32
#endif

MenuSystem::MenuSystem() :
    currentState(MenuState::MAIN_MENU),
    showControls(false),
    showHighscores(false),
    selectedMainMenuItem(0),
    selectedPauseMenuItem(0),
    selectedGameOverMenuItem(0),
    finalScore(0),
    finalTime("00:00") {

    initialize();
}

void MenuSystem::initialize() {
    createMainMenu();
    createPauseMenu();
    createGameOverMenu();
    std::cout << "Menu System initialized" << std::endl;
}

void MenuSystem::createMainMenu() {
    mainMenuItems.clear();

    float startY = 8.0f;
    float itemHeight = 1.5f;
    float itemWidth = 10.0f;
    float centerX = 4.0f;

    mainMenuItems.push_back({
        "START GAME", centerX, startY, itemWidth, itemHeight, false,
        [this]() { setState(MenuState::IN_GAME); }
        });

    mainMenuItems.push_back({
        "CONTROLS", centerX, startY + itemHeight * 1.2f, itemWidth, itemHeight, false,
        [this]() { showControls = true; showHighscores = false; }
        });

    mainMenuItems.push_back({
        "HIGHSCORES", centerX, startY + itemHeight * 2.4f, itemWidth, itemHeight, false,
        [this]() { showHighscores = true; showControls = false; }
        });

    mainMenuItems.push_back({
        "EXIT", centerX, startY + itemHeight * 3.6f, itemWidth, itemHeight, false,
        []() { exit(0); }
        });

    // Set first item as selected
    if (!mainMenuItems.empty()) {
        mainMenuItems[0].isSelected = true;
    }
}

void MenuSystem::createPauseMenu() {
    pauseMenuItems.clear();

    float startY = 9.0f;
    float itemHeight = 1.5f;
    float itemWidth = 10.0f;
    float centerX = 4.0f;

    pauseMenuItems.push_back({
        "RESUME", centerX, startY, itemWidth, itemHeight, false,
        [this]() { setState(MenuState::IN_GAME); }
        });

    pauseMenuItems.push_back({
        "EXIT", centerX, startY + itemHeight * 1.2f, itemWidth, itemHeight, false,
        []() { exit(0); }
        });

    // Set first item as selected
    if (!pauseMenuItems.empty()) {
        pauseMenuItems[0].isSelected = true;
    }
}

void MenuSystem::createGameOverMenu() {
    gameOverMenuItems.clear();

    float startY = 12.0f;
    float itemHeight = 1.5f;
    float itemWidth = 12.0f;
    float centerX = 3.0f;

    gameOverMenuItems.push_back({
        "PLAY AGAIN", centerX, startY, itemWidth, itemHeight, false,
        [this]() { setState(MenuState::IN_GAME); }
        });

    gameOverMenuItems.push_back({
        "MAIN MENU", centerX, startY + itemHeight * 1.2f, itemWidth, itemHeight, false,
        [this]() { setState(MenuState::MAIN_MENU); }
        });

    gameOverMenuItems.push_back({
        "EXIT", centerX, startY + itemHeight * 2.4f, itemWidth, itemHeight, false,
        []() { exit(0); }
        });

    // Set first item as selected
    if (!gameOverMenuItems.empty()) {
        gameOverMenuItems[0].isSelected = true;
    }
}

void MenuSystem::setGameOverInfo(int score, const std::string& time) {
    finalScore = score;
    finalTime = time;
}

void MenuSystem::update() {
    // Update selection states
    if (currentState == MenuState::MAIN_MENU) {
        for (size_t i = 0; i < mainMenuItems.size(); i++) {
            mainMenuItems[i].isSelected = (i == selectedMainMenuItem);
        }
    }
    else if (currentState == MenuState::PAUSE_MENU) {
        for (size_t i = 0; i < pauseMenuItems.size(); i++) {
            pauseMenuItems[i].isSelected = (i == selectedPauseMenuItem);
        }
    }
    else if (currentState == MenuState::GAME_OVER_MENU) {
        for (size_t i = 0; i < gameOverMenuItems.size(); i++) {
            gameOverMenuItems[i].isSelected = (i == selectedGameOverMenuItem);
        }
    }
}

void MenuSystem::handleKeyInput(int key) {
    if (currentState == MenuState::MAIN_MENU) {
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
            moveSelectionUp();
        }
        else if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
            moveSelectionDown();
        }
        else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) {
            selectMenuItem();
        }
    }
    else if (currentState == MenuState::PAUSE_MENU) {
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
            selectedPauseMenuItem = (selectedPauseMenuItem - 1 + pauseMenuItems.size()) % pauseMenuItems.size();
        }
        else if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
            selectedPauseMenuItem = (selectedPauseMenuItem + 1) % pauseMenuItems.size();
        }
        else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) {
            if (selectedPauseMenuItem >= 0 && selectedPauseMenuItem < pauseMenuItems.size()) {
                pauseMenuItems[selectedPauseMenuItem].action();
            }
        }
    }
    else if (currentState == MenuState::GAME_OVER_MENU) {
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
            selectedGameOverMenuItem = (selectedGameOverMenuItem - 1 + gameOverMenuItems.size()) % gameOverMenuItems.size();
        }
        else if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
            selectedGameOverMenuItem = (selectedGameOverMenuItem + 1) % gameOverMenuItems.size();
        }
        else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) {
            if (selectedGameOverMenuItem >= 0 && selectedGameOverMenuItem < gameOverMenuItems.size()) {
                gameOverMenuItems[selectedGameOverMenuItem].action();
            }
        }
    }
}

void MenuSystem::selectMenuItem() {
    if (currentState == MenuState::MAIN_MENU) {
        if (selectedMainMenuItem >= 0 && selectedMainMenuItem < mainMenuItems.size()) {
            mainMenuItems[selectedMainMenuItem].action();
        }
    }
}

void MenuSystem::moveSelectionUp() {
    if (currentState == MenuState::MAIN_MENU) {
        selectedMainMenuItem = (selectedMainMenuItem - 1 + mainMenuItems.size()) % mainMenuItems.size();
    }
}

void MenuSystem::moveSelectionDown() {
    if (currentState == MenuState::MAIN_MENU) {
        selectedMainMenuItem = (selectedMainMenuItem + 1) % mainMenuItems.size();
    }
}

void MenuSystem::setState(MenuState newState) {
    currentState = newState;
    showControls = false;
    showHighscores = false;

    // Reset selections
    if (newState == MenuState::MAIN_MENU) {
        selectedMainMenuItem = 0;
    }
    else if (newState == MenuState::PAUSE_MENU) {
        selectedPauseMenuItem = 0;
    }
    else if (newState == MenuState::GAME_OVER_MENU) {
        selectedGameOverMenuItem = 0;
    }

    std::cout << "Menu state changed to: " << static_cast<int>(newState) << std::endl;
}
#pragma once
#include <string>
#include <vector>
#include <functional>
#include <iostream>

enum class MenuState {
    MAIN_MENU,
    CONTROLS,
    HIGHSCORES,
    NAME_INPUT,
    IN_GAME,
    PAUSE_MENU,
    GAME_OVER_MENU
};

struct MenuItem {
    std::string text;
    float x, y, width, height;
    bool isSelected;
    std::function<void()> action;
};

class MenuSystem {
private:
    MenuState currentState;
    std::vector<MenuItem> mainMenuItems;
    std::vector<MenuItem> pauseMenuItems;
    std::vector<MenuItem> gameOverMenuItems;
    int selectedMainMenuItem;
    int selectedPauseMenuItem;
    int selectedGameOverMenuItem;
    int finalScore;
    std::string finalTime;
    std::string currentPlayerName;
    std::string nameInputBuffer;
    std::vector<std::pair<std::string, int>> highscores;
    double lastKeyPressTime;

public:
    MenuSystem();

    void initialize();
    void update();
    void handleKeyInput(int key, double currentTime);
    void handleKeyInput(int key);
    void handleCharInput(unsigned int codepoint, double currentTime);
    void handleCharInput(unsigned int codepoint);
    void selectMenuItem();
    void moveSelectionUp();
    void moveSelectionDown();
    void setState(MenuState newState);

    void setGameOverInfo(int score, const std::string& time);
    void createGameOverMenu();
    void renderMenu() const;

    MenuState getState() const { return currentState; }
    const std::vector<MenuItem>& getMainMenuItems() const { return mainMenuItems; }
    const std::vector<MenuItem>& getPauseMenuItems() const { return pauseMenuItems; }
    const std::vector<MenuItem>& getGameOverMenuItems() const { return gameOverMenuItems; }
    bool shouldShowControls() const { return currentState == MenuState::CONTROLS; }
    bool shouldShowHighscores() const { return currentState == MenuState::HIGHSCORES; }
    int getSelectedMainMenuItem() const { return selectedMainMenuItem; }
    int getSelectedPauseMenuItem() const { return selectedPauseMenuItem; }
    int getSelectedGameOverMenuItem() const { return selectedGameOverMenuItem; }
    int getFinalScore() const { return finalScore; }
    const std::string& getFinalTime() const { return finalTime; }
    const std::string& getCurrentPlayerName() const { return currentPlayerName; }
    const std::string& getNameInputBuffer() const { return nameInputBuffer; }
    const std::vector<std::pair<std::string, int>>& getHighscores() const { return highscores; }
    void setHighscores(const std::vector<std::pair<std::string, int>>& hs) { highscores = hs; }
    void confirmNameInput() { currentPlayerName = nameInputBuffer; }
    void resetNameInput() { nameInputBuffer.clear(); }

private:
    void createMainMenu();
    void createPauseMenu();
};
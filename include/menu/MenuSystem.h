#pragma once
#include <string>
#include <vector>
#include <functional>
#include <iostream>

enum class MenuState {
    MAIN_MENU,
    CONTROLS,
    HIGHSCORES,
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
    bool showControls;
    bool showHighscores;
    int selectedMainMenuItem;
    int selectedPauseMenuItem;
    int selectedGameOverMenuItem;
    int finalScore;
    std::string finalTime;

public:
    MenuSystem();

    void initialize();
    void update();
    void handleKeyInput(int key);
    void selectMenuItem();
    void moveSelectionUp();
    void moveSelectionDown();
    void setState(MenuState newState);

    // Новые методы
    void setGameOverInfo(int score, const std::string& time);
    void createGameOverMenu();
    void renderMenu() const;

    // Геттеры для новых данных
    MenuState getState() const { return currentState; }
    const std::vector<MenuItem>& getMainMenuItems() const { return mainMenuItems; }
    const std::vector<MenuItem>& getPauseMenuItems() const { return pauseMenuItems; }
    const std::vector<MenuItem>& getGameOverMenuItems() const { return gameOverMenuItems; }
    bool shouldShowControls() const { return showControls; }
    bool shouldShowHighscores() const { return showHighscores; }
    int getSelectedMainMenuItem() const { return selectedMainMenuItem; }
    int getSelectedPauseMenuItem() const { return selectedPauseMenuItem; }
    int getSelectedGameOverMenuItem() const { return selectedGameOverMenuItem; }
    int getFinalScore() const { return finalScore; }
    const std::string& getFinalTime() const { return finalTime; }

private:
    void createMainMenu();
    void createPauseMenu();
};
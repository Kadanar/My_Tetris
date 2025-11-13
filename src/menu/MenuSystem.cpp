#include "menu/MenuSystem.h"
#include <GLFW/glfw3.h>

namespace {
	int wrapIndex(int index, std::size_t size) {
		if (size == 0) {
			return 0;
		}

		int count = static_cast<int>(size);
		index %= count;
		if (index < 0) {
			index += count;
		}
		return index;
	}
}

MenuSystem::MenuSystem() :
	currentState(MenuState::MAIN_MENU),
	selectedMainMenuItem(0),
	selectedPauseMenuItem(0),
	selectedGameOverMenuItem(0),
	finalScore(0),
	finalTime("00:00"),
	lastKeyPressTime(0.0) {

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
		[this]() { resetNameInput(); setState(MenuState::NAME_INPUT); }
		});

	mainMenuItems.push_back({
		"CONTROLS", centerX, startY + itemHeight * 1.2f, itemWidth, itemHeight, false,
		[this]() { setState(MenuState::CONTROLS); }
		});

	mainMenuItems.push_back({
		"HIGHSCORES", centerX, startY + itemHeight * 2.4f, itemWidth, itemHeight, false,
		[this]() { setState(MenuState::HIGHSCORES); }
		});

	mainMenuItems.push_back({
		"EXIT", centerX, startY + itemHeight * 3.6f, itemWidth, itemHeight, false,
		[]() { exit(0); }
		});

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

	if (!gameOverMenuItems.empty()) {
		gameOverMenuItems[0].isSelected = true;
	}
}

void MenuSystem::setGameOverInfo(int score, const std::string& time) {
	finalScore = score;
	finalTime = time;
}

void MenuSystem::update() {
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

void MenuSystem::handleKeyInput(int key, double currentTime) {
	if (currentTime - lastKeyPressTime < 0.2) {
		return;
	}

	if (currentState == MenuState::MAIN_MENU) {
		if (mainMenuItems.empty()) {
			return;
		}

		if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
			moveSelectionUp();
			lastKeyPressTime = currentTime;
		}
		else if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
			moveSelectionDown();
			lastKeyPressTime = currentTime;
		}
		else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) {
			selectMenuItem();
			lastKeyPressTime = currentTime;
		}
	}
	else if (currentState == MenuState::NAME_INPUT) {
		if (key == GLFW_KEY_BACKSPACE) {
			if (!nameInputBuffer.empty()) {
				nameInputBuffer.pop_back();
				lastKeyPressTime = currentTime;
				std::cout << "Backspace pressed. Current name: " << nameInputBuffer << std::endl;
			}
		}
		else if (key == GLFW_KEY_ENTER) {
			if (!nameInputBuffer.empty()) {
				confirmNameInput();
				setState(MenuState::IN_GAME);
				lastKeyPressTime = currentTime;
				std::cout << "Name confirmed: " << currentPlayerName << ". Starting game..." << std::endl;
			}
			else {
				std::cout << "Cannot start game: name is empty" << std::endl;
			}
		}
		else if (key == GLFW_KEY_ESCAPE) {
			setState(MenuState::MAIN_MENU);
			lastKeyPressTime = currentTime;
			std::cout << "Name input cancelled" << std::endl;
		}
	}
	else if (currentState == MenuState::PAUSE_MENU) {
		if (pauseMenuItems.empty()) {
			return;
		}

		if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
			selectedPauseMenuItem = wrapIndex(selectedPauseMenuItem - 1, pauseMenuItems.size());
			lastKeyPressTime = currentTime;
		}
		else if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
			selectedPauseMenuItem = wrapIndex(selectedPauseMenuItem + 1, pauseMenuItems.size());
			lastKeyPressTime = currentTime;
		}
		else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) {
			if (selectedPauseMenuItem >= 0 && selectedPauseMenuItem < static_cast<int>(pauseMenuItems.size())) {
				pauseMenuItems[selectedPauseMenuItem].action();
				lastKeyPressTime = currentTime;
			}
		}
	}
	else if (currentState == MenuState::GAME_OVER_MENU) {
		if (gameOverMenuItems.empty()) {
			return;
		}

		if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
			selectedGameOverMenuItem = wrapIndex(selectedGameOverMenuItem - 1, gameOverMenuItems.size());
			lastKeyPressTime = currentTime;
		}
		else if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
			selectedGameOverMenuItem = wrapIndex(selectedGameOverMenuItem + 1, gameOverMenuItems.size());
			lastKeyPressTime = currentTime;
		}
		else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) {
			if (selectedGameOverMenuItem >= 0 && selectedGameOverMenuItem < static_cast<int>(gameOverMenuItems.size())) {
				gameOverMenuItems[selectedGameOverMenuItem].action();
				lastKeyPressTime = currentTime;
			}
		}
	}
	else if (currentState == MenuState::CONTROLS || currentState == MenuState::HIGHSCORES) {
		if (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE || key == GLFW_KEY_ESCAPE) {
			setState(MenuState::MAIN_MENU);
			lastKeyPressTime = currentTime;
		}
	}
}

void MenuSystem::handleKeyInput(int key) {
	handleKeyInput(key, glfwGetTime());
}

void MenuSystem::handleCharInput(unsigned int codepoint, double currentTime) {
	if (currentState != MenuState::NAME_INPUT) return;

	if (currentTime - lastKeyPressTime < 0.1) {
		return;
	}

	if (nameInputBuffer.size() >= 20) return;
	if ((codepoint >= 'A' && codepoint <= 'Z') ||
		(codepoint >= 'a' && codepoint <= 'z') ||
		(codepoint >= '0' && codepoint <= '9') ||
		codepoint == ' ' || codepoint == '_' || codepoint == '-') {
		nameInputBuffer.push_back(static_cast<char>(codepoint));
		lastKeyPressTime = currentTime;
	}
}

void MenuSystem::handleCharInput(unsigned int codepoint) {
	handleCharInput(codepoint, glfwGetTime());
}

void MenuSystem::selectMenuItem() {
	if (currentState == MenuState::MAIN_MENU) {
		if (!mainMenuItems.empty() &&
			selectedMainMenuItem >= 0 &&
			selectedMainMenuItem < static_cast<int>(mainMenuItems.size())) {
			mainMenuItems[selectedMainMenuItem].action();
		}
	}
}

void MenuSystem::moveSelectionUp() {
	if (currentState == MenuState::MAIN_MENU) {
		if (!mainMenuItems.empty()) {
			selectedMainMenuItem = wrapIndex(selectedMainMenuItem - 1, mainMenuItems.size());
		}
	}
}

void MenuSystem::moveSelectionDown() {
	if (currentState == MenuState::MAIN_MENU) {
		if (!mainMenuItems.empty()) {
			selectedMainMenuItem = wrapIndex(selectedMainMenuItem + 1, mainMenuItems.size());
		}
	}
}

void MenuSystem::setState(MenuState newState) {
	currentState = newState;

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
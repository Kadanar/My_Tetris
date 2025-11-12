#pragma once
#include "game/GameBoard.h"
#include "menu/MenuSystem.h"
#include <GLFW/glfw3.h>
#include <string>

class Renderer {
private:
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;

    // Input states
    bool leftPressed, rightPressed, downPressed, upPressed;
    bool aPressed, dPressed, sPressed, wPressed, qPressed, ePressed, spacePressed;

public:
    Renderer();
    ~Renderer();

    bool initialize();
    void shutdown();
    void render(const GameBoard& board);

    // ����� ������ ��� ����
    void renderMenu(const MenuSystem& menu);
    void renderGameOverMenu(const MenuSystem& menu);

    bool shouldClose();
    void processInput(GameBoard& board);

    // ����� ����� ��� ������� � ����
    GLFWwindow* getWindow() const { return window; }

private:
    void drawBlock(float x, float y, int color);
    void drawChar(float x, float y, char c);
    void drawText(float x, float y, const std::string& text);
    void drawNextPiece(const Tetromino& piece, float startX, float startY);

    // ����� ��������� ������ ��� ����
    void drawMenuItem(const MenuItem& item);
    void drawControlsScreen();
    void drawHighscoresScreen(const MenuSystem& menu);
    void drawNameInputScreen(const MenuSystem& menu);
};
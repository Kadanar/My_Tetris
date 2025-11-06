#pragma once
#include "game/GameBoard.h"

#ifndef APIENTRY
#ifdef _WIN32
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

#ifdef _WIN32
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

class Renderer {
private:
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;

    bool leftPressed, rightPressed, downPressed, upPressed;
    bool aPressed, dPressed, sPressed, wPressed, qPressed, spacePressed;

    void drawBlock(float x, float y, int color);
    void drawText(float x, float y, const char* text);
    void drawNextPiece(const Tetromino& piece, float startX, float startY);
    void drawDigit(float x, float y, int digit);
    void drawString(float x, float y, const std::string& str);

public:
    Renderer();
    ~Renderer();

    bool initialize();
    void shutdown();
    void render(const GameBoard& board);
    bool shouldClose();
    void processInput(GameBoard& board);

    GLFWwindow* getWindow() const { return window; }
};
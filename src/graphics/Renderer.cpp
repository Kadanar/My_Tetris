#include "graphics/Renderer.h"
#include <iostream>
#include <string>
#include <sstream>

Renderer::Renderer() : window(nullptr), windowWidth(600), windowHeight(800),
leftPressed(false), rightPressed(false), downPressed(false), upPressed(false),
aPressed(false), dPressed(false), sPressed(false), wPressed(false),
qPressed(false), spacePressed(false) {
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(windowWidth, windowHeight, "My Tetris", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    std::cout << "GLFW initialized successfully!" << std::endl;
    return true;
}

void Renderer::shutdown() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void Renderer::drawBlock(float x, float y, int color) {
    switch (color) {
    case 1: glColor3f(0.0f, 1.0f, 1.0f); break;
    case 2: glColor3f(1.0f, 1.0f, 0.0f); break;
    case 3: glColor3f(1.0f, 0.0f, 1.0f); break;
    case 4: glColor3f(0.0f, 1.0f, 0.0f); break;
    case 5: glColor3f(1.0f, 0.0f, 0.0f); break;
    case 6: glColor3f(0.0f, 0.0f, 1.0f); break;
    case 7: glColor3f(1.0f, 0.5f, 0.0f); break;
    case 8: glColor3f(1.0f, 1.0f, 1.0f); break;
    default: glColor3f(0.7f, 0.7f, 0.7f); break;
    }

    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + 1.0f, y);
    glVertex2f(x + 1.0f, y + 1.0f);
    glVertex2f(x, y + 1.0f);
    glEnd();

    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + 1.0f, y);
    glVertex2f(x + 1.0f, y + 1.0f);
    glVertex2f(x, y + 1.0f);
    glEnd();
}

void Renderer::drawDigit(float x, float y, int digit) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);

    switch (digit) {
    case 0:
        glVertex2f(x, y); glVertex2f(x + 0.15f, y);
        glVertex2f(x, y); glVertex2f(x, y + 0.3f);
        glVertex2f(x, y + 0.3f); glVertex2f(x + 0.15f, y + 0.3f);
        glVertex2f(x + 0.15f, y); glVertex2f(x + 0.15f, y + 0.3f);
        break;
    case 1:
        glVertex2f(x + 0.15f, y); glVertex2f(x + 0.15f, y + 0.3f);
        break;
    case 2:
        glVertex2f(x, y); glVertex2f(x + 0.15f, y);
        glVertex2f(x + 0.15f, y); glVertex2f(x + 0.15f, y + 0.15f);
        glVertex2f(x, y + 0.15f); glVertex2f(x + 0.15f, y + 0.15f);
        glVertex2f(x, y + 0.15f); glVertex2f(x, y + 0.3f);
        glVertex2f(x, y + 0.3f); glVertex2f(x + 0.15f, y + 0.3f);
        break;
    default:
        glVertex2f(x, y); glVertex2f(x + 0.15f, y);
        glVertex2f(x, y); glVertex2f(x, y + 0.3f);
        glVertex2f(x, y + 0.3f); glVertex2f(x + 0.15f, y + 0.3f);
        glVertex2f(x + 0.15f, y); glVertex2f(x + 0.15f, y + 0.3f);
        break;
    }
    glEnd();
}

void Renderer::drawText(float x, float y, const char* text) {
    // Простая реализация текста
}

void Renderer::drawString(float x, float y, const std::string& str) {
    float currentX = x;
    for (char c : str) {
        if (c >= '0' && c <= '9') {
            drawDigit(currentX, y, c - '0');
            currentX += 0.2f;
        }
        else if (c == ':') {
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_POINTS);
            glVertex2f(currentX, y + 0.1f);
            glVertex2f(currentX, y + 0.2f);
            glEnd();
            currentX += 0.1f;
        }
        else {
            currentX += 0.2f;
        }
    }
}

void Renderer::drawNextPiece(const Tetromino& piece, float startX, float startY) {
    const auto& shape = piece.getShape();
    int pieceColor = piece.getColor();

    float offsetX = startX - shape[0].size() * 0.5f;
    float offsetY = startY - shape.size() * 0.5f;

    for (int y = 0; y < shape.size(); y++) {
        for (int x = 0; x < shape[y].size(); x++) {
            if (shape[y][x]) {
                drawBlock(offsetX + x, offsetY + y, pieceColor);
            }
        }
    }
}

void Renderer::render(const GameBoard& board) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 18.0, 22.0, 0.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Отрисовка игрового поля и фигур...
    // (полный код рендеринга из предыдущих версий)

    glfwSwapBuffers(window);
}

bool Renderer::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Renderer::processInput(GameBoard& board) {
    // Обработка ввода...
    // (полный код обработки ввода из предыдущих версий)

    glfwPollEvents();
}
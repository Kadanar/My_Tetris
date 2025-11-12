#include "graphics/Renderer.h"
#include <iostream>
#include <functional>

namespace {
    constexpr float BoardWidth = 12.0f;
    constexpr float BoardHeight = 22.0f;
    constexpr float PanelX0 = 12.5f;
    constexpr float PanelX1 = 17.5f;
}

Renderer::Renderer() : window(nullptr), windowWidth(800), windowHeight(900),
leftPressed(false), rightPressed(false), downPressed(false), upPressed(false),
aPressed(false), dPressed(false), sPressed(false), wPressed(false),
qPressed(false), ePressed(false), spacePressed(false) {
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "My Tetris", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "Renderer initialized successfully!" << std::endl;
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
    case 1: glColor3f(0.0f, 1.0f, 1.0f); break; // Cyan - I
    case 2: glColor3f(1.0f, 1.0f, 0.0f); break; // Yellow - O
    case 3: glColor3f(1.0f, 0.0f, 1.0f); break; // Magenta - T
    case 4: glColor3f(0.0f, 1.0f, 0.0f); break; // Green - S
    case 5: glColor3f(1.0f, 0.0f, 0.0f); break; // Red - Z
    case 6: glColor3f(0.0f, 0.0f, 1.0f); break; // Blue - J
    case 7: glColor3f(1.0f, 0.5f, 0.0f); break; // Orange - L
    case 8: glColor3f(1.0f, 1.0f, 1.0f); break; // White - Animation
    default: glColor3f(0.7f, 0.7f, 0.7f); break; // Gray
    }

    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + 1.0f, y);
    glVertex2f(x + 1.0f, y + 1.0f);
    glVertex2f(x, y + 1.0f);
    glEnd();

    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + 1.0f, y);
    glVertex2f(x + 1.0f, y + 1.0f);
    glVertex2f(x, y + 1.0f);
    glEnd();
}

void Renderer::drawChar(float x, float y, char c) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    switch (toupper(c)) {
    case 'A':
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.15f, y);
        glVertex2f(x + 0.15f, y); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x + 0.05f, y + 0.25f); glVertex2f(x + 0.25f, y + 0.25f);
        break;
    case 'B':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y); glVertex2f(x + 0.2f, y);
        glVertex2f(x, y + 0.25f); glVertex2f(x + 0.2f, y + 0.25f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.2f, y + 0.5f);
        break;
    case 'C':
        glVertex2f(x + 0.3f, y); glVertex2f(x, y);
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'D':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y); glVertex2f(x + 0.2f, y);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.2f, y + 0.5f);
        glVertex2f(x + 0.2f, y); glVertex2f(x + 0.3f, y + 0.25f);
        glVertex2f(x + 0.2f, y + 0.5f); glVertex2f(x + 0.3f, y + 0.25f);
        break;
    case 'E':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y); glVertex2f(x + 0.3f, y);
        glVertex2f(x, y + 0.25f); glVertex2f(x + 0.2f, y + 0.25f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'F':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y); glVertex2f(x + 0.3f, y);
        glVertex2f(x, y + 0.25f); glVertex2f(x + 0.2f, y + 0.25f);
        break;
    case 'G':
        glVertex2f(x + 0.3f, y); glVertex2f(x, y);
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x + 0.3f, y + 0.5f); glVertex2f(x + 0.3f, y + 0.25f);
        glVertex2f(x + 0.3f, y + 0.25f); glVertex2f(x + 0.2f, y + 0.25f);
        break;
    case 'H':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x + 0.3f, y); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x, y + 0.25f); glVertex2f(x + 0.3f, y + 0.25f);
        break;
    case 'I':
        glVertex2f(x + 0.15f, y); glVertex2f(x + 0.15f, y + 0.5f);
        break;
    case 'J':
        glVertex2f(x + 0.3f, y); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x + 0.3f, y + 0.5f); glVertex2f(x + 0.1f, y + 0.5f);
        glVertex2f(x + 0.1f, y + 0.5f); glVertex2f(x, y + 0.4f);
        break;
    case 'K':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y + 0.25f); glVertex2f(x + 0.3f, y);
        glVertex2f(x, y + 0.25f); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'L':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'M':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y); glVertex2f(x + 0.15f, y + 0.25f);
        glVertex2f(x + 0.15f, y + 0.25f); glVertex2f(x + 0.3f, y);
        glVertex2f(x + 0.3f, y); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'N':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x + 0.3f, y); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'O':
        glVertex2f(x, y); glVertex2f(x + 0.3f, y);
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x + 0.3f, y); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'P':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y); glVertex2f(x + 0.3f, y);
        glVertex2f(x, y + 0.25f); glVertex2f(x + 0.3f, y + 0.25f);
        glVertex2f(x + 0.3f, y); glVertex2f(x + 0.3f, y + 0.25f);
        break;
    case 'Q':
        glVertex2f(x, y); glVertex2f(x + 0.3f, y);
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x + 0.3f, y); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x + 0.15f, y + 0.25f); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'R':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y); glVertex2f(x + 0.3f, y);
        glVertex2f(x, y + 0.25f); glVertex2f(x + 0.3f, y + 0.25f);
        glVertex2f(x + 0.3f, y); glVertex2f(x + 0.3f, y + 0.25f);
        glVertex2f(x, y + 0.25f); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'S':
        glVertex2f(x, y); glVertex2f(x + 0.3f, y);
        glVertex2f(x, y); glVertex2f(x, y + 0.25f);
        glVertex2f(x, y + 0.25f); glVertex2f(x + 0.3f, y + 0.25f);
        glVertex2f(x + 0.3f, y + 0.25f); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'T':
        glVertex2f(x, y); glVertex2f(x + 0.3f, y);
        glVertex2f(x + 0.15f, y); glVertex2f(x + 0.15f, y + 0.5f);
        break;
    case 'U':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x + 0.3f, y); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'V':
        glVertex2f(x, y); glVertex2f(x + 0.15f, y + 0.5f);
        glVertex2f(x + 0.15f, y + 0.5f); glVertex2f(x + 0.3f, y);
        break;
    case 'W':
        glVertex2f(x, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.15f, y + 0.25f);
        glVertex2f(x + 0.15f, y + 0.25f); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x + 0.3f, y); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case 'X':
        glVertex2f(x, y); glVertex2f(x + 0.3f, y + 0.5f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.3f, y);
        break;
    case 'Y':
        glVertex2f(x, y); glVertex2f(x + 0.15f, y + 0.25f);
        glVertex2f(x + 0.15f, y + 0.25f); glVertex2f(x + 0.3f, y);
        glVertex2f(x + 0.15f, y + 0.25f); glVertex2f(x + 0.15f, y + 0.5f);
        break;
    case 'Z':
        glVertex2f(x, y); glVertex2f(x + 0.3f, y);
        glVertex2f(x + 0.3f, y); glVertex2f(x, y + 0.5f);
        glVertex2f(x, y + 0.5f); glVertex2f(x + 0.3f, y + 0.5f);
        break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        // Simple digits
    {
        int digit = c - '0';
        if (digit == 0 || digit == 2 || digit == 3 || digit == 5 || digit == 6 || digit == 8 || digit == 9) {
            glVertex2f(x, y); glVertex2f(x + 0.2f, y);
        }
        if (digit == 0 || digit == 4 || digit == 5 || digit == 6 || digit == 8 || digit == 9) {
            glVertex2f(x, y); glVertex2f(x, y + 0.25f);
        }
        if (digit == 0 || digit == 1 || digit == 2 || digit == 3 || digit == 4 || digit == 7 || digit == 8 || digit == 9) {
            glVertex2f(x + 0.2f, y); glVertex2f(x + 0.2f, y + 0.25f);
        }
        if (digit == 2 || digit == 3 || digit == 4 || digit == 5 || digit == 6 || digit == 8 || digit == 9) {
            glVertex2f(x, y + 0.25f); glVertex2f(x + 0.2f, y + 0.25f);
        }
        if (digit == 0 || digit == 2 || digit == 6 || digit == 8) {
            glVertex2f(x, y + 0.25f); glVertex2f(x, y + 0.5f);
        }
        if (digit == 0 || digit == 1 || digit == 3 || digit == 4 || digit == 5 || digit == 6 || digit == 7 || digit == 8 || digit == 9) {
            glVertex2f(x + 0.2f, y + 0.25f); glVertex2f(x + 0.2f, y + 0.5f);
        }
        if (digit == 0 || digit == 2 || digit == 3 || digit == 5 || digit == 6 || digit == 8 || digit == 9) {
            glVertex2f(x, y + 0.5f); glVertex2f(x + 0.2f, y + 0.5f);
        }
    }
    break;
    case ':':
        glVertex2f(x + 0.1f, y + 0.15f); glVertex2f(x + 0.1f, y + 0.15f);
        glVertex2f(x + 0.1f, y + 0.35f); glVertex2f(x + 0.1f, y + 0.35f);
        break;
    case '-':
        glVertex2f(x, y + 0.25f); glVertex2f(x + 0.2f, y + 0.25f);
        break;
    case '/':
        glVertex2f(x + 0.2f, y); glVertex2f(x, y + 0.5f);
        break;
    case ' ':
        // Space - do nothing
        break;
    }
    glEnd();
}

void Renderer::drawText(float x, float y, const std::string& text) {
    float currentX = x;
    for (char c : text) {
        drawChar(currentX, y, c);
        currentX += 0.4f;
    }
}

void Renderer::drawNextPiece(const Tetromino& piece, float startX, float startY) {
    const auto& shape = piece.getShape();
    float blockSize = 0.8f;

    float width = static_cast<float>(shape[0].size()) * blockSize;
    float height = static_cast<float>(shape.size()) * blockSize;

    float offsetX = startX - width * 0.5f;
    float offsetY = startY - height * 0.5f;

    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(offsetX - 0.3f, offsetY - 0.3f);
    glVertex2f(offsetX + width + 0.3f, offsetY - 0.3f);
    glVertex2f(offsetX + width + 0.3f, offsetY + height + 0.3f);
    glVertex2f(offsetX - 0.3f, offsetY + height + 0.3f);
    glEnd();

    for (int y = 0; y < shape.size(); y++) {
        for (int x = 0; x < shape[y].size(); x++) {
            if (shape[y][x]) {
                drawBlock(offsetX + x * blockSize, offsetY + y * blockSize, piece.getColor());
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

    // Рамка поля
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(BoardWidth, 0.0f);
    glVertex2f(BoardWidth, BoardHeight);
    glVertex2f(0.0f, BoardHeight);
    glEnd();

    // Рамка панели справа
    glColor3f(0.4f, 0.4f, 0.4f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(PanelX0, 0.5f);
    glVertex2f(PanelX1, 0.5f);
    glVertex2f(PanelX1, 21.5f);
    glVertex2f(PanelX0, 21.5f);
    glEnd();

    // Сетка поля
    glColor3f(0.3f, 0.3f, 0.3f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (int x = 1; x < static_cast<int>(BoardWidth); x++) {
        glVertex2f(static_cast<float>(x), 0.0f);
        glVertex2f(static_cast<float>(x), BoardHeight);
    }
    for (int y = 1; y < static_cast<int>(BoardHeight); y++) {
        glVertex2f(0.0f, static_cast<float>(y));
        glVertex2f(BoardWidth, static_cast<float>(y));
    }
    glEnd();

    const auto& gameBoard = board.getBoard();
    int animatedColor = board.getAnimatedLineColor();

    for (int y = 0; y < static_cast<int>(BoardHeight); y++) {
        bool isAnimatingLine = false;
        if (board.isAnimating()) {
            bool lineComplete = true;
            for (int x = 0; x < static_cast<int>(BoardWidth); x++) {
                if (gameBoard[y][x] == 0) {
                    lineComplete = false;
                    break;
                }
            }
            isAnimatingLine = lineComplete;
        }

        for (int x = 0; x < static_cast<int>(BoardWidth); x++) {
            if (gameBoard[y][x] != 0) {
                int color = isAnimatingLine ? animatedColor : gameBoard[y][x];
                drawBlock(static_cast<float>(x), static_cast<float>(y), color);
            }
        }
    }

    // Текущая фигура
    if (!board.isAnimating()) {
        const auto& currentPiece = board.getCurrentPiece();
        const auto& shape = currentPiece.getShape();
        int pieceX = currentPiece.getX();
        int pieceY = currentPiece.getY();

        for (int y = 0; y < static_cast<int>(shape.size()); y++) {
            for (int x = 0; x < static_cast<int>(shape[y].size()); x++) {
                if (shape[y][x]) {
                    drawBlock(static_cast<float>(pieceX + x), static_cast<float>(pieceY + y), currentPiece.getColor());
                }
            }
        }
    }

    // === ПАНЕЛЬ ИНФОРМАЦИИ ===

    // Время
    glColor3f(0.4f, 0.4f, 0.6f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(13.0f, 0.8f);
    glVertex2f(17.0f, 0.8f);
    glVertex2f(17.0f, 2.5f);
    glVertex2f(13.0f, 2.5f);
    glEnd();

    drawText(13.0f, 1.0f, "TIME:");
    std::string timeStr = board.getFormattedTime();
    drawText(14.5f, 2.0f, timeStr);

    // Следующая фигура
    glColor3f(0.4f, 0.4f, 0.6f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(13.0f, 3.5f);
    glVertex2f(17.0f, 3.5f);
    glVertex2f(17.0f, 8.0f);
    glVertex2f(13.0f, 8.0f);
    glEnd();

    drawText(13.0f, 4.0f, "NEXT:");
    drawNextPiece(board.getNextPiece(), 15.0f, 6.0f);

    // Счет и уровень
    glColor3f(0.4f, 0.4f, 0.6f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(13.0f, 8.5f);
    glVertex2f(17.0f, 8.5f);
    glVertex2f(17.0f, 11.5f);
    glVertex2f(13.0f, 11.5f);
    glEnd();

    drawText(13.0f, 9.0f, "SCORE:");
    drawText(14.5f, 10.0f, std::to_string(board.getScore()));
    drawText(13.0f, 10.8f, "LEVEL: " + std::to_string(board.getLevel()));

    // Статус игры
    glColor3f(0.6f, 0.4f, 0.4f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(13.0f, 18.5f);
    glVertex2f(17.0f, 18.5f);
    glVertex2f(17.0f, 20.5f);
    glVertex2f(13.0f, 20.5f);
    glEnd();

    if (board.isGamePaused()) {
        drawText(13.5f, 19.5f, "PAUSED");
    }
    else if (board.isGameOver()) {
        drawText(13.0f, 19.5f, "GAME OVER");
    }

    glfwSwapBuffers(window);
}

void Renderer::renderMenu(const MenuSystem& menu) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 18.0, 22.0, 0.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw title
    drawText(5.0f, 3.0f, "MY TETRIS");

    if (menu.getState() == MenuState::NAME_INPUT) {
        drawNameInputScreen(menu);
    }
    else if (menu.shouldShowControls()) {
        drawControlsScreen();
    }
    else if (menu.shouldShowHighscores()) {
        drawHighscoresScreen(menu);
    }
    else {
        // Draw menu items
        const auto& items = menu.getMainMenuItems();
        for (const auto& item : items) {
            drawMenuItem(item);
        }
    }

    glfwSwapBuffers(window);
}

void Renderer::renderGameOverMenu(const MenuSystem& menu) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 18.0, 22.0, 0.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw title
    drawText(5.0f, 3.0f, "GAME OVER");

    // Draw final score and time
    drawText(4.0f, 5.0f, "FINAL SCORE: " + std::to_string(menu.getFinalScore()));
    drawText(4.0f, 6.5f, "TIME: " + menu.getFinalTime());

    // Draw menu items
    const auto& items = menu.getGameOverMenuItems();
    for (const auto& item : items) {
        drawMenuItem(item);
    }

    glfwSwapBuffers(window);
}

void Renderer::drawMenuItem(const MenuItem& item) {
    if (item.isSelected) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow for selected
    }
    else {
        glColor3f(1.0f, 1.0f, 1.0f); // White for normal
    }

    drawText(item.x, item.y, item.text);

    // Draw selection indicator
    if (item.isSelected) {
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(item.x - 0.5f, item.y + 0.25f);
        glVertex2f(item.x - 0.3f, item.y + 0.5f);
        glVertex2f(item.x - 0.3f, item.y + 0.0f);
        glEnd();
    }
}

void Renderer::drawControlsScreen() {
    drawText(4.0f, 5.0f, "CONTROLS:");
    drawText(3.0f, 6.5f, "ARROWS/WASD - MOVE");
    drawText(3.0f, 7.5f, "W/UP - ROTATE");
    drawText(3.0f, 8.5f, "S/DOWN - FAST DROP");
    drawText(3.0f, 9.5f, "E/SPACE - HARD DROP");
    drawText(3.0f, 10.5f, "Q - PAUSE");
    drawText(3.0f, 11.5f, "ESC - QUIT");
    drawText(3.0f, 13.0f, "PRESS ANY KEY TO RETURN");
}

void Renderer::drawHighscoresScreen(const MenuSystem& menu) {
    drawText(5.0f, 5.0f, "HIGHSCORES");
    float y = 7.0f;
    int rank = 1;
    for (const auto& row : menu.getHighscores()) {
        std::string line = std::to_string(rank) + ". " + row.first + " - " + std::to_string(row.second);
        drawText(3.5f, y, line);
        y += 1.0f;
        rank++;
        if (rank > 10) break;
    }
    if (rank == 1) {
        drawText(4.0f, 7.5f, "NO SCORES YET");
    }
    drawText(3.0f, 13.0f, "PRESS ANY KEY TO RETURN");
}

void Renderer::drawNameInputScreen(const MenuSystem& menu) {
    drawText(4.0f, 6.0f, "ENTER YOUR NAME:");
    std::string input = menu.getNameInputBuffer();
    if (input.empty()) input = "_";
    drawText(4.0f, 8.0f, input);
    drawText(3.0f, 10.0f, "ENTER - CONFIRM, ESC - CANCEL, BACKSPACE - DELETE");
}

void Renderer::processInput(GameBoard& board) {
    // Handle movement
    auto handleKey = [&](int key, bool& pressed, std::function<void()> action) {
        if (glfwGetKey(window, key) == GLFW_PRESS && !pressed) {
            action();
            pressed = true;
        }
        else if (glfwGetKey(window, key) == GLFW_RELEASE) {
            pressed = false;
        }
        };

    handleKey(GLFW_KEY_LEFT, leftPressed, [&]() { board.movePieceLeft(); });
    handleKey(GLFW_KEY_RIGHT, rightPressed, [&]() { board.movePieceRight(); });
    handleKey(GLFW_KEY_A, aPressed, [&]() { board.movePieceLeft(); });
    handleKey(GLFW_KEY_D, dPressed, [&]() { board.movePieceRight(); });

    handleKey(GLFW_KEY_UP, upPressed, [&]() { board.rotatePiece(); });
    handleKey(GLFW_KEY_W, wPressed, [&]() { board.rotatePiece(); });

    // Fast drop
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !downPressed) {
        board.setFastDrop(true);
        downPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE && downPressed) {
        board.setFastDrop(false);
        downPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !sPressed) {
        board.setFastDrop(true);
        sPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && sPressed) {
        board.setFastDrop(false);
        sPressed = false;
    }

    handleKey(GLFW_KEY_E, ePressed, [&]() { board.hardDrop(); });
    handleKey(GLFW_KEY_SPACE, spacePressed, [&]() { board.hardDrop(); });

    // Pause: handled in main to keep state in sync
    qPressed = (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS);

    // Exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    glfwPollEvents();
}

bool Renderer::shouldClose() {
    return glfwWindowShouldClose(window);
}
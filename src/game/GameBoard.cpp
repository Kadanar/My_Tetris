#include "game/GameBoard.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

GameBoard::GameBoard() : score(0), gameOver(false), gamePaused(false),
linesToClear(0), animationTimer(0), gameTimer(0),
fastDrop(false) {
    board.resize(HEIGHT, std::vector<int>(WIDTH, 0));
    nextPiece = Tetromino::getRandomTetromino();
    spawnNewPiece();
}

void GameBoard::spawnNewPiece() {
    currentPiece = nextPiece;
    nextPiece = Tetromino::getRandomTetromino();

    if (!isValidMove(currentPiece, currentPiece.getX(), currentPiece.getY())) {
        gameOver = true;
    }
}

bool GameBoard::isValidMove(const Tetromino& piece, int newX, int newY) const {
    const auto& shape = piece.getShape();

    for (int y = 0; y < shape.size(); y++) {
        for (int x = 0; x < shape[y].size(); x++) {
            if (shape[y][x]) {
                int boardX = newX + x;
                int boardY = newY + y;

                if (boardX < 0 || boardX >= WIDTH || boardY >= HEIGHT) {
                    return false;
                }

                if (boardY >= 0 && board[boardY][boardX] != 0) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool GameBoard::movePieceLeft() {
    if (isValidMove(currentPiece, currentPiece.getX() - 1, currentPiece.getY())) {
        currentPiece.moveLeft();
        return true;
    }
    return false;
}

bool GameBoard::movePieceRight() {
    if (isValidMove(currentPiece, currentPiece.getX() + 1, currentPiece.getY())) {
        currentPiece.moveRight();
        return true;
    }
    return false;
}

bool GameBoard::movePieceDown() {
    if (isValidMove(currentPiece, currentPiece.getX(), currentPiece.getY() + 1)) {
        currentPiece.moveDown();
        return true;
    }
    return false;
}

bool GameBoard::rotatePiece() {
    Tetromino rotated = currentPiece;
    rotated.rotate();

    if (isValidMove(rotated, rotated.getX(), rotated.getY())) {
        currentPiece = rotated;
        return true;
    }
    return false;
}

void GameBoard::lockPiece() {
    const auto& shape = currentPiece.getShape();
    int pieceX = currentPiece.getX();
    int pieceY = currentPiece.getY();

    for (int y = 0; y < shape.size(); y++) {
        for (int x = 0; x < shape[y].size(); x++) {
            if (shape[y][x]) {
                int boardY = pieceY + y;
                if (boardY >= 0) {
                    board[boardY][pieceX + x] = currentPiece.getColor();
                }
            }
        }
    }

    score += clearLines();
    spawnNewPiece();
}

int GameBoard::clearLines() {
    std::vector<int> linesToRemove;

    for (int y = HEIGHT - 1; y >= 0; y--) {
        bool lineComplete = true;

        for (int x = 0; x < WIDTH; x++) {
            if (board[y][x] == 0) {
                lineComplete = false;
                break;
            }
        }

        if (lineComplete) {
            linesToRemove.push_back(y);
        }
    }

    if (!linesToRemove.empty()) {
        linesToClear = static_cast<int>(linesToRemove.size());
        animationTimer = 0;
        return linesToClear * 100;
    }

    return 0;
}

void GameBoard::update(double deltaTime) {
    if (gamePaused || gameOver || linesToClear > 0) {
        if (linesToClear > 0) {
            updateAnimation(deltaTime);
        }
        return;
    }

    gameTimer += deltaTime;

    double dropInterval = fastDrop ? 0.1 : 0.5;

    static double timeSinceLastDrop = 0;
    timeSinceLastDrop += deltaTime;

    if (timeSinceLastDrop >= dropInterval) {
        if (!movePieceDown()) {
            int linesScore = clearLines();
            if (linesScore == 0) {
                spawnNewPiece();
            }
            else {
                score += linesScore;
            }
        }
        timeSinceLastDrop = 0;
    }
}

void GameBoard::updateAnimation(double deltaTime) {
    if (linesToClear > 0) {
        animationTimer += deltaTime;

        if (animationTimer >= 1.0) {
            for (int y = HEIGHT - 1; y >= 0; y--) {
                bool lineComplete = true;
                for (int x = 0; x < WIDTH; x++) {
                    if (board[y][x] == 0) {
                        lineComplete = false;
                        break;
                    }
                }

                if (lineComplete) {
                    for (int yy = y; yy > 0; yy--) {
                        board[yy] = board[yy - 1];
                    }
                    board[0] = std::vector<int>(WIDTH, 0);
                    y++;
                }
            }

            linesToClear = 0;
            animationTimer = 0;
        }
    }
}

int GameBoard::getAnimatedLineColor() const {
    if (linesToClear == 0) return 0;

    int colorIndex = static_cast<int>(animationTimer * 10) % 8;
    return colorIndex + 1;
}

void GameBoard::togglePause() {
    gamePaused = !gamePaused;
}

std::string GameBoard::getFormattedTime() const {
    int totalSeconds = static_cast<int>(gameTimer);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;
    return oss.str();
}
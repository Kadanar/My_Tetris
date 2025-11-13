#include "game/GameBoard.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>

namespace {
    std::mt19937& rng() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }
}

GameBoard::GameBoard() : score(0), gameOver(false), gamePaused(false),
linesToClear(0), animationTimer(0), gameTimer(0),
fastDrop(false), timeSinceLastDrop(0) {
    board.resize(HEIGHT, std::vector<int>(WIDTH, 0));
    refillBag();
    nextPiece = Tetromino(popNextType());
    spawnNewPiece();
}

void GameBoard::spawnNewPiece() {
    currentPiece = nextPiece;
    nextPiece = Tetromino(popNextType());
    int spawnX = (WIDTH - static_cast<int>(currentPiece.getShape()[0].size())) / 2;
    currentPiece.setPosition(spawnX, 0);
    pieceCounts[static_cast<int>(currentPiece.getType())]++;

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
    if (tryRotateWithKicks(rotated)) {
        currentPiece = rotated;
        return true;
    }
    return false;
}

void GameBoard::lockPiece() {
    const auto& shape = currentPiece.getShape();
    int pieceX = currentPiece.getX();
    int pieceY = currentPiece.getY();
    int pieceColor = currentPiece.getColor();

    for (int y = 0; y < shape.size(); y++) {
        for (int x = 0; x < shape[y].size(); x++) {
            if (shape[y][x]) {
                int boardY = pieceY + y;
                if (boardY >= 0) {
                    board[boardY][pieceX + x] = pieceColor;
                }
            }
        }
    }

    int gained = clearLines();
    updateLevelByLines(gained > 0 ? linesToClear : 0);
    if (linesToClear == 0) {
        spawnNewPiece();
    }
}

int GameBoard::clearLines() {
    linesToRemove.clear();

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

        int points = 0;
        switch (linesToClear) {
        case 1: points = 100; break;
        case 2: points = 300; break;
        case 3: points = 500; break;
        case 4: points = 800; break;
        }
        score += points;
        totalClearedLines += linesToClear;

        return points;
    }

    return 0;
}

void GameBoard::update(double deltaTime) {
    if (gamePaused || gameOver) {
        return;
    }

    if (linesToClear > 0) {
        updateAnimation(deltaTime);
        return;
    }

    gameTimer += deltaTime;
    timeSinceLastDrop += deltaTime;

    double levelFactor = std::max(0.1, 1.0 - 0.08 * (level - 1));
    double normalInterval = baseDropInterval * levelFactor;
    double dropInterval = fastDrop ? fastDropInterval : normalInterval;

    if (timeSinceLastDrop >= dropInterval) {
        if (!movePieceDown()) {
            lockPiece();
        }
        timeSinceLastDrop = 0;
    }
}

void GameBoard::updateAnimation(double deltaTime) {
    if (linesToClear > 0) {
        animationTimer += deltaTime;

        if (animationTimer >= 0.5) {
            std::sort(linesToRemove.begin(), linesToRemove.end(), std::greater<int>());

            for (int lineY : linesToRemove) {
                for (int yy = lineY; yy > 0; yy--) {
                    board[yy] = board[yy - 1];
                }
                board[0] = std::vector<int>(WIDTH, 0);
            }

            linesToClear = 0;
            linesToRemove.clear();
            animationTimer = 0;

            spawnNewPiece();
        }
    }
}

int GameBoard::getAnimatedLineColor() const {
    if (linesToClear == 0) return 0;
    int colorIndex = static_cast<int>(animationTimer * 10) % 8;
    return colorIndex + 1;
}

void GameBoard::hardDrop() {
    while (movePieceDown()) {
    }
    lockPiece();
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

void GameBoard::refillBag() {
    std::vector<TetrominoType> bag = {
        TetrominoType::I, TetrominoType::O, TetrominoType::T,
        TetrominoType::S, TetrominoType::Z, TetrominoType::J, TetrominoType::L
    };
    std::shuffle(bag.begin(), bag.end(), rng());
    for (auto t : bag) {
        pieceQueue.push(t);
    }
}

TetrominoType GameBoard::popNextType() {
    if (pieceQueue.empty()) {
        refillBag();
    }
    TetrominoType t = pieceQueue.front();
    pieceQueue.pop();
    return t;
}

bool GameBoard::tryRotateWithKicks(Tetromino& rotated) const {
    static const int kicks[][2] = { {-1,0}, {1,0}, {0,-1}, {-2,0}, {2,0} };
    for (auto& k : kicks) {
        int nx = rotated.getX() + k[0];
        int ny = rotated.getY() + k[1];
        if (isValidMove(rotated, nx, ny)) {
            rotated.setPosition(nx, ny);
            return true;
        }
    }
    return false;
}

void GameBoard::updateLevelByLines(int clearedNow) {
    if (clearedNow <= 0) return;
    int newLevel = 1 + totalClearedLines / 10;
    if (newLevel > level) {
        level = newLevel;
    }
}
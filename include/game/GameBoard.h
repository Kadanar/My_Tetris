#pragma once
#include "Tetromino.h"
#include <vector>
#include <string>
#include <queue>

class GameBoard {
private:
    static const int WIDTH = 12;
    static const int HEIGHT = 22;

    std::vector<std::vector<int>> board;
    Tetromino currentPiece;
    Tetromino nextPiece;
    int score;
    bool gameOver;
    bool gamePaused;
    int linesToClear;
    std::vector<int> linesToRemove;
    double animationTimer;
    double gameTimer;
    bool fastDrop;
    double timeSinceLastDrop;

    int level = 1;
    int totalClearedLines = 0;

    std::queue<TetrominoType> pieceQueue;

    double baseDropInterval = 0.8;
    double fastDropInterval = 0.05;

    int pieceCounts[7] = { 0,0,0,0,0,0,0 };

    void refillBag();
    TetrominoType popNextType();
    bool tryRotateWithKicks(Tetromino& rotated) const;
    void updateLevelByLines(int clearedNow);

public:
    GameBoard();

    void spawnNewPiece();
    bool isValidMove(const Tetromino& piece, int newX, int newY) const;
    bool movePieceLeft();
    bool movePieceRight();
    bool movePieceDown();
    bool rotatePiece();
    void lockPiece();
    int clearLines();
    void update(double deltaTime);

    void updateAnimation(double deltaTime);
    int getAnimatedLineColor() const;

    bool isGameOver() const { return gameOver; }
    bool isGamePaused() const { return gamePaused; }
    void togglePause() { gamePaused = !gamePaused; }
    void setPaused(bool paused) { gamePaused = paused; }
    int getScore() const { return score; }
    const std::vector<std::vector<int>>& getBoard() const { return board; }
    const Tetromino& getCurrentPiece() const { return currentPiece; }
    const Tetromino& getNextPiece() const { return nextPiece; }
    int getWidth() const { return WIDTH; }
    int getHeight() const { return HEIGHT; }
    bool isAnimating() const { return linesToClear > 0; }

    double getGameTime() const { return gameTimer; }
    void setFastDrop(bool fast) { fastDrop = fast; }
    int getLevel() const { return level; }
    int getTotalClearedLines() const { return totalClearedLines; }
    std::string getFormattedTime() const;
    void hardDrop();
    const int* getPieceCounts() const { return pieceCounts; }
};
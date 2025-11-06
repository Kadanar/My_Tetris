#pragma once
#include "Tetromino.h"
#include <vector>
#include <string>

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
    double animationTimer;
    double gameTimer;
    bool fastDrop;

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
    void togglePause();
    int getScore() const { return score; }
    const std::vector<std::vector<int>>& getBoard() const { return board; }
    const Tetromino& getCurrentPiece() const { return currentPiece; }
    const Tetromino& getNextPiece() const { return nextPiece; }
    int getWidth() const { return WIDTH; }
    int getHeight() const { return HEIGHT; }
    bool isAnimating() const { return linesToClear > 0; }

    double getGameTime() const { return gameTimer; }
    void setFastDrop(bool fast) { fastDrop = fast; }
    std::string getFormattedTime() const;
};
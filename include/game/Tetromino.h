#pragma once
#include <vector>

enum class TetrominoType {
    I, O, T, S, Z, J, L
};

class Tetromino {
private:
    TetrominoType type;
    std::vector<std::vector<bool>> shape;
    int x, y;
    int color;

public:
    Tetromino(TetrominoType tetrominoType = TetrominoType::I);

    void rotate();
    void moveLeft();
    void moveRight();
    void moveDown();

    const std::vector<std::vector<bool>>& getShape() const { return shape; }
    int getX() const { return x; }
    int getY() const { return y; }
    TetrominoType getType() const { return type; }
    int getColor() const { return color; }

    void setPosition(int newX, int newY) { x = newX; y = newY; }

    static Tetromino getRandomTetromino();
};
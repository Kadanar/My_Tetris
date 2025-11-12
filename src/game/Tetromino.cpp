#include "game/Tetromino.h"
#include <random>

Tetromino::Tetromino(TetrominoType tetrominoType) : type(tetrominoType), x(4), y(0) {
    switch (type) {
    case TetrominoType::I:
        shape = { {1, 1, 1, 1} };
        color = 1;
        // x уже установлен в списке инициализации
        break;
    case TetrominoType::O:
        shape = { {1, 1}, {1, 1} };
        color = 2;
        break;
    case TetrominoType::T:
        shape = { {0, 1, 0}, {1, 1, 1} };
        color = 3;
        break;
    case TetrominoType::S:
        shape = { {0, 1, 1}, {1, 1, 0} };
        color = 4;
        break;
    case TetrominoType::Z:
        shape = { {1, 1, 0}, {0, 1, 1} };
        color = 5;
        break;
    case TetrominoType::J:
        shape = { {1, 0, 0}, {1, 1, 1} };
        color = 6;
        break;
    case TetrominoType::L:
        shape = { {0, 0, 1}, {1, 1, 1} };
        color = 7;
        break;
    }
}

void Tetromino::rotate() {
    if (shape.empty()) return;

    size_t newRows = shape[0].size();
    size_t newCols = shape.size();
    std::vector<std::vector<bool>> newShape(newRows, std::vector<bool>(newCols, false));

    for (size_t i = 0; i < shape.size(); i++) {
        for (size_t j = 0; j < shape[i].size(); j++) {
            newShape[j][shape.size() - 1 - i] = shape[i][j];
        }
    }

    shape = newShape;
}

void Tetromino::moveLeft() { x--; }
void Tetromino::moveRight() { x++; }
void Tetromino::moveDown() { y++; }

Tetromino Tetromino::getRandomTetromino() {
    // Используем random_device для лучшей случайности
    static std::random_device rd;
    static std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, 6);
    return Tetromino(static_cast<TetrominoType>(dist(rng)));
}
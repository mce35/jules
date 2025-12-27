#ifndef TETROMINO_HPP
#define TETROMINO_HPP

#include <vector>
#include <string>
#include "Constants.hpp"

class Tetromino {
public:
    int x, y;
    int rotation;
    int shape_index;

    Tetromino(int start_x, int start_y, int idx);

    std::vector<std::string> get_shape() const;
    std::vector<double> get_color() const;
    void rotate();
};

#endif // TETROMINO_HPP

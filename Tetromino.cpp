#include "Tetromino.hpp"

Tetromino::Tetromino(int start_x, int start_y, int idx)
    : x(start_x), y(start_y), rotation(0), shape_index(idx) {}

std::vector<std::string> Tetromino::get_shape() const {
    const auto& all_rotations = SHAPES[this->shape_index];
    return all_rotations[this->rotation % all_rotations.size()];
}

std::vector<double> Tetromino::get_color() const {
    return SHAPE_COLORS[this->shape_index];
}

void Tetromino::rotate() {
    const auto& all_rotations = SHAPES[this->shape_index];
    this->rotation = (this->rotation + 1) % all_rotations.size();
}

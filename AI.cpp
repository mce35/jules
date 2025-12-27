#include "AI.hpp"
#include <limits>
#include <cmath>

AI::AI(const Board& b) : board(b) {}

std::pair<int, int> AI::get_best_move(const Tetromino& piece) {
    int best_x = 0;
    int best_rotation = 0;
    double best_score = std::numeric_limits<double>::lowest();

    for (int rotation = 0; rotation < SHAPES[piece.shape_index].size(); ++rotation) {
        for (int x = -2; x < 11; ++x) {
            Tetromino temp_piece = piece;
            temp_piece.rotation = rotation;
            temp_piece.x = x;
            temp_piece.y = 0;

            if (!this->board.valid_space(temp_piece)) continue;

            while (this->board.valid_space(temp_piece)) {
                temp_piece.y++;
            }
            temp_piece.y--;

            double score = this->score_move(temp_piece);
            if (score > best_score) {
                best_score = score;
                best_x = x;
                best_rotation = rotation;
            }
        }
    }
    return {best_x, best_rotation};
}

double AI::score_move(const Tetromino& piece) const {
    Board temp_board = this->board;
    temp_board.lock_piece(piece);

    int lines_cleared = this->_calculate_lines_cleared(temp_board.grid);
    int aggregate_height = this->_calculate_aggregate_height(temp_board.grid);
    int holes = this->_calculate_holes(temp_board.grid);
    int bumpiness = this->_calculate_bumpiness(temp_board.grid);

    return (w_height * aggregate_height) +
           (w_lines * lines_cleared) +
           (w_holes * holes) +
           (w_bumpiness * bumpiness);
}

std::vector<int> AI::_get_column_heights(const std::vector<std::vector<Color>>& grid) const {
    std::vector<int> heights(10, 0);
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 20; ++y) {
            if (grid[y][x].r != 0.0) {
                heights[x] = 20 - y;
                break;
            }
        }
    }
    return heights;
}

int AI::_calculate_aggregate_height(const std::vector<std::vector<Color>>& grid) const {
    auto heights = this->_get_column_heights(grid);
    int total_height = 0;
    for (int h : heights) total_height += h;
    return total_height;
}

int AI::_calculate_lines_cleared(const std::vector<std::vector<Color>>& grid) const {
    int lines = 0;
    for (int y = 0; y < 20; ++y) {
        bool full = true;
        for (int x = 0; x < 10; ++x) {
            if (grid[y][x].r == 0.0) {
                full = false;
                break;
            }
        }
        if (full) lines++;
    }
    return lines * lines;
}

int AI::_calculate_holes(const std::vector<std::vector<Color>>& grid) const {
    int holes = 0;
    for (int x = 0; x < 10; ++x) {
        bool block_found = false;
        for (int y = 0; y < 20; ++y) {
            if (grid[y][x].r != 0.0) {
                block_found = true;
            } else if (block_found) {
                holes++;
            }
        }
    }
    return holes;
}

int AI::_calculate_bumpiness(const std::vector<std::vector<Color>>& grid) const {
    auto heights = this->_get_column_heights(grid);
    int bumpiness = 0;
    for (size_t i = 0; i < heights.size() - 1; ++i) {
        bumpiness += std::abs(heights[i] - heights[i + 1]);
    }
    return bumpiness;
}

#include "Board.hpp"
#include "Constants.hpp"
#include <algorithm>

Board::Board() : score(0) {
    this->create_grid();
}

void Board::create_grid() {
    this->grid.assign(20, std::vector<Color>(10, {0.0, 0.0, 0.0}));
    for (const auto& pos_color_pair : this->locked_positions) {
        const auto& pos = pos_color_pair.first;
        const auto& color = pos_color_pair.second;
        if (pos.second >= 0 && pos.second < 20 && pos.first >= 0 && pos.first < 10) {
            this->grid[pos.second][pos.first] = color;
        }
    }
}

std::vector<std::pair<int, int>> Board::convert_shape_format(const Tetromino& shape) const {
    std::vector<std::pair<int, int>> positions;
    auto shape_template = shape.get_shape();

    for (int i = 0; i < shape_template.size(); ++i) {
        for (int j = 0; j < shape_template[i].size(); ++j) {
            if (shape_template[i][j] == '0') {
                positions.push_back({shape.x + j - 2, shape.y + i - 4});
            }
        }
    }
    return positions;
}

bool Board::valid_space(const Tetromino& shape) const {
    auto formatted_shape = this->convert_shape_format(shape);
    for (const auto& pos : formatted_shape) {
        if (pos.first < 0 || pos.first >= 10 || pos.second >= 20) {
            return false;
        }
        if (pos.second >= 0 && this->grid[pos.second][pos.first].r != 0.0) {
            return false;
        }
    }
    return true;
}

bool Board::check_lost() const {
    for (const auto& pos_color_pair : this->locked_positions) {
        if (pos_color_pair.first.second < 1) {
            return true;
        }
    }
    return false;
}

void Board::clear_rows() {
    int rows_cleared = 0;
    std::vector<int> full_rows;

    // First, find all the full rows from bottom to top
    for (int i = 19; i >= 0; --i) {
        bool row_full = true;
        for (int j = 0; j < 10; ++j) {
            if (this->grid[i][j].r == 0.0) { // Check for an empty cell
                row_full = false;
                break;
            }
        }
        if (row_full) {
            full_rows.push_back(i);
            rows_cleared++;
        }
    }

    if (rows_cleared == 0) {
        return; // Nothing to do
    }

    // Update score
    this->score += rows_cleared * 10 * rows_cleared; // Bonus for multi-line clears

    // Create a new map for locked positions
    std::map<std::pair<int, int>, Color> new_locked_positions;

    // Copy over the blocks that are not in the cleared rows
    for (const auto& pair : this->locked_positions) {
        bool in_cleared_row = false;
        for (int row_y : full_rows) {
            if (pair.first.second == row_y) {
                in_cleared_row = true;
                break;
            }
        }
        if (!in_cleared_row) {
            // Count how many cleared rows are below this block
            int shift_down = 0;
            for (int row_y : full_rows) {
                if (row_y > pair.first.second) {
                    shift_down++;
                }
            }
            new_locked_positions[{pair.first.first, pair.first.second + shift_down}] = pair.second;
        }
    }

    this->locked_positions = new_locked_positions;
}

void Board::lock_piece(const Tetromino& shape) {
    auto color_vec = shape.get_color();
    Color piece_color = {color_vec[0], color_vec[1], color_vec[2]};
    for (const auto& pos : this->convert_shape_format(shape)) {
        this->locked_positions[pos] = piece_color;
    }
    this->clear_rows();
    this->create_grid();
}

#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <map>
#include "Tetromino.hpp"

// Using a struct for the color for better readability
struct Color {
    double r, g, b;
};

class Board {
public:
    Board();

    std::vector<std::vector<Color>> grid;
    int score;

    std::vector<std::pair<int, int>> convert_shape_format(const Tetromino& shape) const;
    bool valid_space(const Tetromino& shape) const;
    bool check_lost() const;
    void clear_rows();
    void lock_piece(const Tetromino& shape);

private:
    std::map<std::pair<int, int>, Color> locked_positions;
    void create_grid();
};

#endif // BOARD_HPP

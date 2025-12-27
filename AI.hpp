#ifndef AI_HPP
#define AI_HPP

#include "Board.hpp"
#include "Tetromino.hpp"
#include <utility>

class AI {
public:
    AI(const Board& b);

    std::pair<int, int> get_best_move(const Tetromino& piece);

private:
    const Board& board;

    // Heuristic weights
    double w_height = -0.510066;
    double w_lines = 0.760666;
    double w_holes = -0.35663;
    double w_bumpiness = -0.184483;

    double score_move(const Tetromino& piece) const;
    int _calculate_aggregate_height(const std::vector<std::vector<Color>>& grid) const;
    int _calculate_lines_cleared(const std::vector<std::vector<Color>>& grid) const;
    int _calculate_holes(const std::vector<std::vector<Color>>& grid) const;
    int _calculate_bumpiness(const std::vector<std::vector<Color>>& grid) const;
    std::vector<int> _get_column_heights(const std::vector<std::vector<Color>>& grid) const;
};

#endif // AI_HPP

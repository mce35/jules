#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <vector>
#include <string>
#include <map>

// Screen dimensions
constexpr int WINDOW_WIDTH = 1040;
constexpr int WINDOW_HEIGHT = 700;

constexpr int PLAY_WIDTH = 300;
constexpr int PLAY_HEIGHT = 600;
constexpr int BLOCK_SIZE = 30;

// Derived dimensions
constexpr int PLAYER_TOP_LEFT_X = (WINDOW_WIDTH / 2 - PLAY_WIDTH) - 50;
constexpr int PLAYER_TOP_LEFT_Y = WINDOW_HEIGHT - PLAY_HEIGHT - 20;

constexpr int AI_TOP_LEFT_X = (WINDOW_WIDTH / 2) + 50;
constexpr int AI_TOP_LEFT_Y = WINDOW_HEIGHT - PLAY_HEIGHT - 20;

// Tetromino Shapes (a vector of shapes, where each shape is a vector of rotations)
const std::vector<std::vector<std::vector<std::string>>> SHAPES = {
    // S
    {{".....", ".....", "..00.", ".00..", "....."}, {".....", "..0..", "..00.", "...0.", "....."}},
    // Z
    {{".....", ".....", ".00..", "..00.", "....."}, {".....", "..0..", ".00..", ".0...", "....."}},
    // I
    {{"..0..", "..0..", "..0..", "..0..", "....."}, {".....", "0000.", ".....", ".....", "....."}},
    // O
    {{".....", ".....", ".00..", ".00..", "....."}},
    // J
    {{".....", ".0...", ".000.", ".....", "....."}, {".....", "..00.", "..0..", "..0..", "....."},
     {".....", ".....", ".000.", "...0.", "....."}, {".....", "..0..", "..0..", ".00..", "....."}},
    // L
    {{".....", "...0.", ".000.", ".....", "....."}, {".....", "..0..", "..0..", "..00.", "....."},
     {".....", ".....", ".000.", ".0...", "....."}, {".....", ".00..", "..0..", "..0..", "....."}},
    // T
    {{".....", "..0..", ".000.", ".....", "....."}, {".....", "..0..", "..00.", "..0..", "....."},
     {".....", ".....", ".000.", "..0..", "....."}, {".....", "..0..", ".00..", "..0..", "....."}},
};

// Shape Colors (RGB values normalized to 0.0-1.0 for Cairo)
const std::vector<std::vector<double>> SHAPE_COLORS = {
    {0.0, 1.0, 0.0},   // S - Green
    {1.0, 0.0, 0.0},   // Z - Red
    {0.0, 1.0, 1.0},   // I - Cyan
    {1.0, 1.0, 0.0},   // O - Yellow
    {1.0, 0.65, 0.0},  // J - Orange
    {0.0, 0.0, 1.0},   // L - Blue
    {0.5, 0.0, 0.5}    // T - Purple
};

#endif // CONSTANTS_HPP

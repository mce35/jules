#include "GameArea.hpp"
#include "Constants.hpp"
#include <cairomm/context.h>
#include <gtkmm/stylecontext.h>

GameArea::GameArea(Board& b, const Tetromino& current, const Tetromino& next)
    : board(b), current_piece(current), next_piece(next), m_title("Player") {

    // Request a size for this widget
    set_size_request(PLAY_WIDTH + 200, WINDOW_HEIGHT);

    // Configure fonts once
    m_font_title.set_family("Monospace");
    m_font_title.set_weight(Pango::WEIGHT_BOLD);
    m_font_title.set_size(30 * PANGO_SCALE);

    m_font_score.set_family("Monospace");
    m_font_score.set_size(18 * PANGO_SCALE);

    m_font_next = m_font_score;
}

GameArea::~GameArea() {}

void GameArea::set_title(const std::string& title) {
    m_title = title;
}

bool GameArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    // Black background
    cr->set_source_rgb(0.1, 0.1, 0.1);
    cr->paint();

    int text_width, text_height;

    // --- Draw Title ---
    auto title_layout = create_pango_layout(m_title);
    title_layout->set_font_description(m_font_title);
    title_layout->get_pixel_size(text_width, text_height);
    cr->set_source_rgb(1.0, 1.0, 1.0);
    // Center the title horizontally within the play area part of the widget
    cr->move_to((PLAY_WIDTH / 2) - (text_width / 2) + 20, 30);
    title_layout->show_in_cairo_context(cr);

    // --- Draw Score ---
    auto score_layout = create_pango_layout("Score: " + std::to_string(board.score));
    score_layout->set_font_description(m_font_score);
    cr->move_to(PLAY_WIDTH + 40, 150);
    score_layout->show_in_cairo_context(cr);

    // --- Main drawing area for the board ---
    cr->save();
    // The main translation point for the play area
    cr->translate(20, 80);

    draw_grid(cr);
    draw_board(cr);
    draw_piece(cr, current_piece);

    // Draw border around play area
    cr->set_source_rgb(1.0, 0.0, 0.0);
    cr->set_line_width(4.0);
    cr->rectangle(0, 0, PLAY_WIDTH, PLAY_HEIGHT);
    cr->stroke();

    cr->restore();

    // --- Draw next piece area ---
    cr->save();
    cr->translate(PLAY_WIDTH + 40, 200);
    draw_next_piece_area(cr);
    cr->restore();

    return true;
}

void GameArea::draw_grid(const Cairo::RefPtr<Cairo::Context>& cr) {
    cr->set_line_width(1.0);
    cr->set_source_rgba(0.5, 0.5, 0.5, 0.5); // Light gray for grid lines

    for (int i = 1; i < 10; ++i) { // Vertical lines
        cr->move_to(i * BLOCK_SIZE, 0);
        cr->line_to(i * BLOCK_SIZE, PLAY_HEIGHT);
    }
    for (int i = 1; i < 20; ++i) { // Horizontal lines
        cr->move_to(0, i * BLOCK_SIZE);
        cr->line_to(PLAY_WIDTH, i * BLOCK_SIZE);
    }
    cr->stroke();
}

void GameArea::draw_board(const Cairo::RefPtr<Cairo::Context>& cr) {
    for (int y = 0; y < 20; ++y) {
        for (int x = 0; x < 10; ++x) {
            Color c = board.grid[y][x];
            if (c.r != 0.0 || c.g != 0.0 || c.b != 0.0) {
                cr->set_source_rgb(c.r, c.g, c.b);
                cr->rectangle(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                cr->fill();
            }
        }
    }
}

void GameArea::draw_piece(const Cairo::RefPtr<Cairo::Context>& cr, const Tetromino& piece) {
    auto color_vec = piece.get_color();
    cr->set_source_rgb(color_vec[0], color_vec[1], color_vec[2]);

    auto positions = board.convert_shape_format(piece);
    for (const auto& pos : positions) {
        if (pos.second >= 0) { // Only draw blocks that are on the board
            cr->rectangle(pos.first * BLOCK_SIZE, pos.second * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
            cr->fill();
        }
    }
}

void GameArea::draw_next_piece_area(const Cairo::RefPtr<Cairo::Context>& cr) {
    // Title
    auto layout = create_pango_layout("Next Shape");
    layout->set_font_description(m_font_next);
    cr->set_source_rgb(1.0, 1.0, 1.0);
    layout->show_in_cairo_context(cr);

    // Draw the piece
    Tetromino preview_piece = next_piece;
    preview_piece.x = 2;
    preview_piece.y = 5; // Adjust position for preview

    auto color_vec = preview_piece.get_color();
    cr->set_source_rgb(color_vec[0], color_vec[1], color_vec[2]);
    auto positions = board.convert_shape_format(preview_piece);

    for (const auto& pos : positions) {
        // We ignore the board's y-position and draw relative to the "Next Shape" text
        cr->rectangle(pos.first * BLOCK_SIZE, (pos.second - 1) * BLOCK_SIZE + 40, BLOCK_SIZE, BLOCK_SIZE);
        cr->fill();
    }
}

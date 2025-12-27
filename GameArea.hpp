#ifndef GAME_AREA_HPP
#define GAME_AREA_HPP

#include <gtkmm/drawingarea.h>
#include "Board.hpp"
#include "Tetromino.hpp"
#include <pangomm/layout.h>
#include <pangomm/fontdescription.h>

class GameArea : public Gtk::DrawingArea {
public:
    GameArea(Board& b, const Tetromino& current, const Tetromino& next);
    virtual ~GameArea();

    void set_title(const std::string& title);

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

private:
    Board& board;
    const Tetromino& current_piece;
    const Tetromino& next_piece;
    std::string m_title;

    // Pango resources for drawing text
    Pango::FontDescription m_font_title;
    Pango::FontDescription m_font_score;
    Pango::FontDescription m_font_next;

    void draw_grid(const Cairo::RefPtr<Cairo::Context>& cr);
    void draw_board(const Cairo::RefPtr<Cairo::Context>& cr);
    void draw_piece(const Cairo::RefPtr<Cairo::Context>& cr, const Tetromino& piece);
    void draw_next_piece_area(const Cairo::RefPtr<Cairo::Context>& cr);
};

#endif // GAME_AREA_HPP

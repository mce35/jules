#include <gtkmm.h>
#include <random>
#include "Board.hpp"
#include "Tetromino.hpp"
#include "AI.hpp"
#include "GameArea.hpp"
#include "Constants.hpp"

class GameWindow : public Gtk::Window {
public:
    GameWindow();
    virtual ~GameWindow();

protected:
    // Signal handlers
    bool on_key_press_event(GdkEventKey* event) override;
    bool on_timeout();

private:
    // Game state
    Board player_board;
    Board ai_board;

    Tetromino player_current_piece;
    Tetromino player_next_piece;

    Tetromino ai_current_piece;
    Tetromino ai_next_piece;

    AI ai;
    int ai_target_x;
    int ai_target_rotation;

    // Helper methods
    int get_random_shape_idx();
    void reset_player_piece();
    void reset_ai_piece();

    // Widgets
    Gtk::Box m_main_box;
    GameArea m_player_area;
    GameArea m_ai_area;
};

GameWindow::GameWindow() :
    player_board(),
    ai_board(),
    player_current_piece(5, 0, get_random_shape_idx()),
    player_next_piece(5, 0, get_random_shape_idx()),
    ai_current_piece(5, 0, get_random_shape_idx()),
    ai_next_piece(5, 0, get_random_shape_idx()),
    ai(ai_board),
    ai_target_x(0),
    ai_target_rotation(0),
    m_main_box(Gtk::ORIENTATION_HORIZONTAL, 20),
    m_player_area(player_board, player_current_piece, player_next_piece),
    m_ai_area(ai_board, ai_current_piece, ai_next_piece)
{
    set_title("Tetris AI");
    set_default_size(WINDOW_WIDTH, WINDOW_HEIGHT);
    set_resizable(false);

    m_player_area.set_title("Player");
    m_ai_area.set_title("AI");

    m_main_box.set_halign(Gtk::ALIGN_CENTER);
    m_main_box.pack_start(m_player_area, Gtk::PACK_SHRINK);
    m_main_box.pack_start(m_ai_area, Gtk::PACK_SHRINK);
    add(m_main_box);

    // This is crucial for capturing key presses
    add_events(Gdk::KEY_PRESS_MASK);

    // Get the first move for the AI
    auto best_move = ai.get_best_move(ai_current_piece);
    ai_target_x = best_move.first;
    ai_target_rotation = best_move.second;

    // Start the game loop
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &GameWindow::on_timeout), 300); // 300ms interval

    show_all_children();
}

GameWindow::~GameWindow() {}

int GameWindow::get_random_shape_idx() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distrib(0, SHAPES.size() - 1);
    return distrib(gen);
}

void GameWindow::reset_player_piece() {
    player_current_piece = player_next_piece;
    player_next_piece = Tetromino(5, 0, get_random_shape_idx());
}

void GameWindow::reset_ai_piece() {
    ai_current_piece = ai_next_piece;
    ai_next_piece = Tetromino(5, 0, get_random_shape_idx());

    // Get the best move and instantly apply the rotation and horizontal position
    auto best_move = ai.get_best_move(ai_current_piece);
    ai_current_piece.x = best_move.first;
    ai_current_piece.rotation = best_move.second;

    // Check if the new position is valid, if not, the AI loses.
    if (!ai_board.valid_space(ai_current_piece)) {
        // This case should be rare but is a safeguard
        Gtk::MessageDialog dialog(*this, "Game Over", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
        dialog.set_secondary_text("You Won! (AI failed to place piece)");
        dialog.run();
        // Ideally, you'd handle stopping the timer here.
    }
}

bool GameWindow::on_key_press_event(GdkEventKey* event) {
    Tetromino temp_piece = player_current_piece;
    switch (event->keyval) {
        case GDK_KEY_Left:
            temp_piece.x--;
            break;
        case GDK_KEY_Right:
            temp_piece.x++;
            break;
        case GDK_KEY_Down:
            temp_piece.y++;
            break;
        case GDK_KEY_Up:
            temp_piece.rotate();
            break;
        default:
            return Gtk::Window::on_key_press_event(event);
    }

    if (player_board.valid_space(temp_piece)) {
        player_current_piece = temp_piece;
        m_player_area.queue_draw();
    } else if (event->keyval == GDK_KEY_Up) {
        // If rotation fails, it might be against a wall, try to "wall kick"
        temp_piece = player_current_piece;
        temp_piece.rotate();
        temp_piece.x++; // Kick right
        if(player_board.valid_space(temp_piece)) {
            player_current_piece = temp_piece;
            m_player_area.queue_draw();
        } else {
            temp_piece.x -= 2; // Kick left
             if(player_board.valid_space(temp_piece)) {
                player_current_piece = temp_piece;
                m_player_area.queue_draw();
            }
        }
    }
    return true;
}

bool GameWindow::on_timeout() {
    // --- Player Logic ---
    Tetromino temp_player_piece = player_current_piece;
    temp_player_piece.y++;
    if (player_board.valid_space(temp_player_piece)) {
        player_current_piece.y++;
    } else {
        player_board.lock_piece(player_current_piece);
        reset_player_piece();
        if (player_board.check_lost()) {
            Gtk::MessageDialog dialog(*this, "Game Over", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
            dialog.set_secondary_text("You Lost!");
            dialog.run();
            return false; // Stop timer
        }
    }

    // --- AI Logic ---
    Tetromino temp_ai_piece = ai_current_piece;
    temp_ai_piece.y++;
    if (ai_board.valid_space(temp_ai_piece)) {
        ai_current_piece.y++;
    } else {
        ai_board.lock_piece(ai_current_piece);
        reset_ai_piece();
        if (ai_board.check_lost()) {
            Gtk::MessageDialog dialog(*this, "Game Over", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
            dialog.set_secondary_text("You Won!");
            dialog.run();
            return false; // Stop timer
        }
    }

    // Request a redraw of both areas
    m_player_area.queue_draw();
    m_ai_area.queue_draw();

    return true; // Keep timer running
}


int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.cpp-tetris.app");

    GameWindow window;

    return app->run(window);
}

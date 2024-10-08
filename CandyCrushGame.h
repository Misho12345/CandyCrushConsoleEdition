#pragma once
#include "CandyMatrix.h"
#include "utils/print.h"

#include <variant>
#include <thread>

#ifdef UNIX
#include <termios.h>
#endif

class CandyCrushGame
{
public:
    CandyCrushGame(uint32_t width, uint32_t height);

    void play();

private:
    enum class InputKey
    {
        Enter,
        Space
    };

    class Env
    {
    public:
        Env();
        ~Env();

        #ifdef UNIX
        termios old_t{};
        #endif
    };

    void display_ui() const;
    void refresh_score();
    void refresh_pixel(const Position& pos) const;

    void refresh(bool is_in_setup = false);

    void handle_movement(const Direction& dir);
    void move_cursor(const Position& pos);
    void swap(const Position& other_pos);

    [[nodiscard]] std::variant<Direction, InputKey> get_input();

    static void            send_cursor_to_top();
    [[nodiscard]] STRING_T get_quit_pos() const;


    Env         env{};
    CandyMatrix matrix;

    Position cursor{ 0, 0 };
    bool     swap_mode  = false;
    bool     refreshing = false;

    uint32_t score            = 0;
    uint32_t display_score    = 0;
    bool     refreshing_score = false;
};

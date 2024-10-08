#include "CandyCrushGame.h"

#ifdef UNIX
#include <unistd.h>
#else
#include <conio.h>
#endif

#ifndef FORCE_ASCII
#include <locale>
#endif

using namespace std::chrono_literals;

CandyCrushGame::Env::Env()
{
    #ifndef FORCE_ASCII
    std::locale::global(std::locale("en_US.UTF-8"));
    #endif

    #ifdef UNIX
    system("clear");
    tcgetattr(STDIN_FILENO, &old_t);
    termios new_t = old_t;

    new_t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_t);
    new_t.c_cc[VMIN]  = 1;
    new_t.c_cc[VTIME] = 0;

    #else
    system("cls");
    #endif
}

CandyCrushGame::Env::~Env()
{
    #ifdef UNIX
    tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
    #endif
}


CandyCrushGame::CandyCrushGame(const uint32_t width, const uint32_t height)
    : matrix(width, height, [this](const Position& pos) { refresh_pixel(pos); })
{
    display_ui();
    refresh(true);
}


void CandyCrushGame::play()
{
    while (true)
    {
        const auto input      = get_input();
        const bool is_outside = cursor.y == matrix.get_height();

        if (std::holds_alternative<Direction>(input))
        {
            const auto dir = std::get<Direction>(input);
            handle_movement(dir);
            continue;
        }

        if (const auto key = std::get<InputKey>(input); key == InputKey::Enter)
        {
            if (!is_outside) continue;

            print(STRING("{}\n"), get_quit_pos());
            flush();
            break;
        }
        else if (key == InputKey::Space && !is_outside && !refreshing)
        {
            swap_mode = !swap_mode;
            refresh_pixel(cursor);
            send_cursor_to_top();
        }
    }
}


void CandyCrushGame::display_ui() const
{
    constexpr uint8_t tl_r = 255, tl_b = 0,   tl_g = 0,
                      tr_r = 255, tr_b = 255, tr_g = 0,
                      bl_r = 255, bl_b = 0,   bl_g = 255,
                      br_r = 0,   br_b = 255, br_g = 255;

    auto lerp = [](const uint8_t a, const uint8_t b, const double t) { return static_cast<uint8_t>(a + t * (b - a)); };

    for (uint32_t i = 0; i < matrix.get_height() + 2; i++)
    {
        const double t = i / static_cast<double>(matrix.get_height() + 2);

        STRING_T left = colored(
            lerp(tl_r, bl_r, t),
            lerp(tl_g, bl_g, t),
            lerp(tl_b, bl_b, t),
            i == 0 ? STRING("┌") : i == matrix.get_height() + 1 ? STRING("└") : STRING("│"));

        STRING_T right = colored(
            lerp(tr_r, br_r, t),
            lerp(tr_g, br_g, t),
            lerp(tr_b, br_b, t),
            i == 0 ? STRING("┐") : i == matrix.get_height() + 1 ? STRING("┘") : STRING("│"));

        print(STRING("\033[{};{}H{}"), i + 1, 1, left);
        print(STRING("\033[{};{}H{}"), i + 1, matrix.get_width() * 2 + 3, right);
    }

    for (uint32_t i = 0; i < matrix.get_width() * 2 + 1; i++)
    {
        const double t = (i + 1) / static_cast<double>(matrix.get_width() * 2 + 3);

        STRING_T top = colored(
            lerp(tl_r, tr_r, t),
            lerp(tl_g, tr_g, t),
            lerp(tl_b, tr_b, t),
            STRING("─"));

        STRING_T bottom = colored(
            lerp(bl_r, br_r, t),
            lerp(bl_g, br_g, t),
            lerp(bl_b, br_b, t),
            STRING("─"));

        print(STRING("\033[{};{}H{}"), 1, i + 2, top);
        print(STRING("\033[{};{}H{}"), matrix.get_height() + 2, i + 2, bottom);
    }

    print(STRING("{} Quit "), get_quit_pos());

    print(STRING("\033[{};{}HScore: {}"),
          2, matrix.get_width() * 2 + 6,
          colored(220, 210, 30, TO_STRING(score)));

    std::array sign =
    {
        colored(230, 230, 50, STRING("   ____                       _                 ")),
        colored(230, 215, 50, STRING("  / ___|   __ _   _ __     __| |  _   _         ")),
        colored(230, 200, 50, STRING(" | |      / _` | | '_ \\   / _` | | | | |       ")),
        colored(230, 185, 50, STRING(" | |___  | (_| | | | | | | (_| | | |_| |        ")),
        colored(230, 170, 50, STRING("  \\____|  \\__,_| |_| |_|  \\__,_|  \\__, |    ")),
        colored(230, 155, 50, STRING("   ____                        _  |___/         ")),
        colored(230, 140, 50, STRING("  / ___|  _ __   _   _   ___  | |__   "  )) + colored(230, 208, 50, STRING("  ____   ")),
        colored(230, 125, 50, STRING(" | |     | '__| | | | | / __| | '_ \\  " )) + colored(190, 216, 70, STRING(" |___ \\ ")),
        colored(230, 110, 50, STRING(" | |___  | |    | |_| | \\__ \\ | | | | ")) + colored(150, 224, 70, STRING("   __) | ")),
        colored(230,  95, 50, STRING("  \\____| |_|     \\__,_| |___/ |_| |_| ")) + colored(110, 232, 70, STRING("  / __/  ")),
        colored(230,  80, 50, STRING("                                      "  )) + colored( 70, 240, 70, STRING(" |_____| "))
    };

    for (uint32_t i = 0; i < sign.size(); i++)
        print(STRING("\033[{};{}H{}"), 4 + i, matrix.get_width() * 2 + 5, sign[i]);

    flush();
}

void CandyCrushGame::refresh_score()
{
    if (refreshing_score) return;
    refreshing_score = true;

    std::thread([this]
    {
        const uint8_t incr = (score < display_score ? -1 : 1) * 13;

        while (true)
        {
            if (display_score > score) display_score = score;

            print(STRING("\033[{};{}H{}"),
                  2, matrix.get_width() * 2 + 13,
                  colored(220, 210, 30, TO_STRING(display_score)));

            send_cursor_to_top();
            std::this_thread::sleep_for(50ms);

            if (display_score == score) break;
            display_score += incr;
        }

        refreshing_score = false;
    }).detach();
}

void CandyCrushGame::refresh_pixel(const Position& pos) const
{
    auto print_pixel = [this](const Position& pos_, const bool selected)
    {
        print(STRING("\033[{};{}H{}{}{}"),
                  pos_.y + 2, pos_.x * 2 + 2,
                  selected ? (swap_mode ? '<' : '[') : ' ',
                  matrix.get(pos_) != nullptr ? matrix.get(pos_)->to_string() : STRING(" "),
                  selected ? (swap_mode ? '>' : ']') : ' ');
    };

    const bool selected = cursor == pos;
    print_pixel(pos, selected);

    if (!selected && (
            std::abs(static_cast<int64_t>(cursor.x) - pos.x) <= 1 ||
            std::abs(static_cast<int64_t>(cursor.y) - pos.y) <= 1) &&
        cursor.y < matrix.get_height())
        print_pixel(cursor, true);

    send_cursor_to_top();
}


void CandyCrushGame::refresh(const bool is_in_setup)
{
    if (refreshing) return;
    refreshing = true;

    uint32_t temp;
    uint32_t& score_ = is_in_setup ? temp : score;

    std::thread([&]
    {
        do
        {
            do if (!is_in_setup) std::this_thread::sleep_for(150ms);
            while (matrix.bring_down());

            if (!is_in_setup)
            {
                std::this_thread::sleep_for(200ms);
                refresh_score();
            }

            send_cursor_to_top();
        }
        while (matrix.remove_matches(score_));

        refreshing = false;
    }).detach();
}


void CandyCrushGame::handle_movement(const Direction& dir)
{
    const Position curr_pos  = cursor;
    const Position other_pos = cursor + dir;

    if (!other_pos.in_bounds(matrix.get_width(), matrix.get_height() + 1))
    {
        swap_mode = false;
        return;
    }

    const bool curr_outside  = curr_pos.y == matrix.get_height();
    const bool other_outside = other_pos.y == matrix.get_height();

    if (other_outside && (swap_mode || curr_outside)) return;

    if (curr_outside)
    {
        cursor = other_pos;
        print(STRING("{} Quit "), get_quit_pos());
        refresh_pixel(other_pos);
    }
    else if (other_outside)
    {
        cursor    = other_pos;
        print(STRING("{}[Quit]"), get_quit_pos());
        refresh_pixel(curr_pos);
    }
    else if (!swap_mode) move_cursor(other_pos);
    else swap(other_pos);
}

void CandyCrushGame::move_cursor(const Position& pos)
{
    const Position curr = cursor;
    cursor              = pos;

    refresh_pixel(curr);
    refresh_pixel(cursor);
}

void CandyCrushGame::swap(const Position& other_pos)
{
    const Position curr_pos = cursor;

    matrix.swap(curr_pos, other_pos);
    move_cursor(other_pos);

    std::this_thread::sleep_for(200ms);

    // this is made to prioritize candy destruction before bomb destruction,
    // otherwise the bomb would destroy the candy before it could be destroyed
    if (!instance_of<Candy>(matrix.get(curr_pos)) &&
        instance_of<Candy>(matrix.get(other_pos)) &&
        !matrix.get(other_pos)->on_move(matrix, other_pos, score) &&
        !matrix.get(curr_pos)->on_move(matrix, curr_pos, score)
        ||
        !matrix.get(curr_pos)->on_move(matrix, curr_pos, score) &&
        !matrix.get(other_pos)->on_move(matrix, other_pos, score))
    {
        std::this_thread::sleep_for(100ms);
        matrix.swap(curr_pos, other_pos);
        move_cursor(curr_pos);
    }
    else
    {
        if (matrix.get(curr_pos) != nullptr && matrix.get(curr_pos)->destroy_on_move()) matrix.destroy(curr_pos);
        if (matrix.get(other_pos) != nullptr && matrix.get(other_pos)->destroy_on_move()) matrix.destroy(other_pos);
        refresh_score();
        refresh();
    }

    swap_mode = false;
    refresh_pixel(cursor);
}


std::variant<Direction, CandyCrushGame::InputKey> CandyCrushGame::get_input()
{
    unsigned char ch;

    while (true)
    {
        #ifdef UNIX
        if (read(STDIN_FILENO, &ch, 1) != 1) continue;

        switch (ch)
        {
            case 'w': return Direction::up();
            case 'a': return Direction::left();
            case 's': return Direction::down();
            case 'd': return Direction::right();

            case '\n': return InputKey::Enter;
            case ' ': return InputKey::Space;

            default: break;
        }

        if (ch != '\033') continue;

        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1 || seq[0] != '[') continue;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) continue;

        switch (seq[1])
        {
            case 'A': return Direction::up();
            case 'B': return Direction::down();
            case 'C': return Direction::right();
            case 'D': return Direction::left();
            default: break;
        }

        #else
        ch = _getch();

        switch (ch)
        {
            case 'w': return Direction::up();
            case 'a': return Direction::left();
            case 's': return Direction::down();
            case 'd': return Direction::right();

            case '\r': return InputKey::Enter;
            case ' ': return InputKey::Space;

            default: break;
        }

        if (ch != 0 && ch != 224) continue;
        ch = _getch();
        switch (ch)
        {
            case 72: return return Direction::up();
            case 80: return return Direction::down();
            case 77: return Direction::right();
            case 75: return Direction::left();
            default: break;
        }
        #endif
    }
}


void CandyCrushGame::send_cursor_to_top()
{
    print(STRING("\033[1;1H"));
    flush();
}

STRING_T CandyCrushGame::get_quit_pos() const
{
    return std::format(STRING("\033[{};{}H"),
                       matrix.get_height() + 2,
                       matrix.get_width() * 2 - 4);
}


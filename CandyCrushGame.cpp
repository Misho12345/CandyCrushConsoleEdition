#include "CandyCrushGame.h"
#include "Bomb.h"

#ifndef UNIX
#include <conio.h>
#endif

using namespace std::chrono_literals;


CandyCrushGame::CandyCrushGame(const uint32_t width, const uint32_t height)
    : candies(width, height, [this](const uint32_t x, const uint32_t y) { refresh_pixel(x, y); })
{
    for (uint32_t x = 0; x < width; x++)
    {
        for (uint32_t y = 0; y < height; y++)
            candies.set(new Candy(CandyMatrix::rand_color()), x, y);
    }

    display_ui();
    refresh(true);
}

CandyCrushGame::~CandyCrushGame()
{
    for (uint32_t x = 0; x < candies.get_width(); x++)
    {
        for (uint32_t y = 0; y < candies.get_height(); y++)
            if (candies.get(x, y) != nullptr)
                delete candies.get(x, y);
    }
}


void CandyCrushGame::run()
{
    while (true)
    {
        const auto input      = get_input();
        const bool is_outside = cursor.y >= candies.get_height();

        if (std::holds_alternative<InputKey>(input))
        {
            if (const auto key = std::get<InputKey>(input); key == InputKey::Enter)
            {
                if (!is_outside) continue;

                print(STRING("{}\n\r"), get_bottom_pos());
                flush();
                break;
            }
            else if (key == InputKey::Space && !is_outside && !refreshing)
            {
                swap_mode = !swap_mode;
                refresh_pixel(cursor);
            }

            continue;
        }

        const auto dir = std::get<Direction>(input);
        handle_movement(dir);
    }
}


void CandyCrushGame::display_ui() const
{
    printnl(STRING("{} - Quit\033[{};{}HScore: {}"),
            get_bottom_pos(),
            2, candies.get_width() * 2 + 5,
            colored(220, 210, 30, TO_STRING(score)));
    flush();
}

void CandyCrushGame::refresh_score(const uint32_t new_score)
{
    display_score = score;
    score         = new_score;

    if (refreshing_score) return;
    refreshing_score = true;

    std::thread([this]
    {
        do
        {
            constexpr uint32_t step = 2;

            display_score += step;
            if (display_score > score) display_score = score;

            print(STRING("\033[{};{}H{}"),
                  2, candies.get_width() * 2 + 12,
                  colored(220, 210, 30, TO_STRING(display_score)));

            send_cursor_to_top();

            std::this_thread::sleep_for(30ms);
        }
        while (display_score != score);

        refreshing_score = false;
    }).detach();
}


void CandyCrushGame::refresh_pixel(const uint32_t x, const uint32_t y) const
{
    refresh_pixel(Position{ x, y });
}

void CandyCrushGame::refresh_pixel(const Position& pos) const
{
    const bool selected = cursor == pos;

    print(STRING("\033[{};{}H{}{}{}"),
          pos.y + 2, pos.x * 2 + 2,
          selected ? (swap_mode ? '<' : '[') : ' ',
          candies.get(pos.x, pos.y) != nullptr ? candies.get(pos.x, pos.y)->to_string() : STRING(" "),
          selected ? (swap_mode ? '>' : ']') : ' ');

    if (!selected && (
            std::abs(cursor.x - pos.x) <= 1 ||
            std::abs(cursor.y - pos.y) <= 1) &&
        cursor.y < candies.get_height())
        refresh_pixel(cursor);

    flush();
}


void CandyCrushGame::send_cursor_to_top()
{
    print(STRING("\033[1;1H"));
    flush();
}


STRING_T CandyCrushGame::get_bottom_pos() const { return std::format(STRING("\033[{};1H"), candies.get_height() + 2); }


void CandyCrushGame::handle_movement(const Direction& dir)
{
    const Position curr_pos  = cursor;
    const Position other_pos = cursor + dir;

    const bool is_outside      = curr_pos.y >= candies.get_height();
    const bool will_be_outside = other_pos.y >= candies.get_height();

    if (will_be_outside && (swap_mode || is_outside)) return;
    if (!will_be_outside && !is_in_bounds(other_pos)) return;

    if (is_outside)
    {
        cursor = other_pos;

        print(STRING("{} -"), get_bottom_pos());
        refresh_pixel(other_pos);
        send_cursor_to_top();
    }
    else if (will_be_outside)
    {
        cursor    = other_pos;
        swap_mode = false;

        print(STRING("{} >"), get_bottom_pos());
        refresh_pixel(curr_pos);
        send_cursor_to_top();
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
    send_cursor_to_top();
}

void CandyCrushGame::swap(const Position& other_pos)
{
    const Position curr_pos = cursor;

    candies.swap(curr_pos.x, curr_pos.y, other_pos.x, other_pos.y);
    move_cursor(other_pos);

    std::this_thread::sleep_for(200ms);

    if (!remove_matches())
    {
        candies.swap(curr_pos.x, curr_pos.y, other_pos.x, other_pos.y);
        move_cursor(curr_pos);
    }
    else refresh();

    swap_mode = false;
    refresh_pixel(cursor);
}

bool CandyCrushGame::is_in_bounds(const Position& pos) const
{
    return pos.x >= 0 && pos.x < static_cast<int64_t>(candies.get_width()) &&
            pos.y >= 0 && pos.y < static_cast<int64_t>(candies.get_height());
}

std::variant<CandyCrushGame::Direction, CandyCrushGame::InputKey> CandyCrushGame::get_input()
{
    unsigned char ch;

    while (true)
    {
        #ifdef UNIX
        if (read(STDIN_FILENO, &ch, 1) != 1) continue;

        if (ch == '\n') return InputKey::Enter;
        if (ch == ' ') return InputKey::Space;

        if (ch != '\033') continue;

        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1 || seq[0] != '[') continue;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) continue;

        switch (seq[1])
        {
            case 'A': return directions[3]; // Up
            case 'B': return directions[1]; // Down
            case 'C': return directions[0]; // Right
            case 'D': return directions[2]; // Left
            default: break;
        }

        #else

        ch = _getch();

        if (ch == 13) return InputKey::Enter;
        if (ch == 32) return InputKey::Space;

        if (ch != 0 && ch != 224) continue;
        ch = _getch();
        switch (ch)
        {
            case 72: return directions[3]; // Up
            case 80: return directions[1]; // Down
            case 77: return directions[0]; // Right
            case 75: return directions[2]; // Left
            default: break;
        }
        #endif
    }
}


std::vector<CandyCrushGame::Surroundings> CandyCrushGame::check_surroundings(const Position& pos) const
{
    std::vector<Surroundings> vec;

    for (const auto& dir : directions)
    {
        uint32_t length = 1;
        Position pos_   = pos + dir;

        if (!is_in_bounds(pos_) ||
            !is_in_bounds(pos + dir * 3))
            continue;

        if (candies.get(pos.x, pos.y) == nullptr ||
            candies.get(pos.x, pos.y)->is_bomb())
            continue;

        const auto candy = reinterpret_cast<Candy*>(candies.get(pos.x, pos.y));

        while (
            is_in_bounds(pos_) &&
            candies.get(pos_.x, pos_.y) != nullptr &&
            !candies.get(pos_.x, pos_.y)->is_bomb() &&
            *candy == *reinterpret_cast<Candy*>(candies.get(pos_.x, pos_.y))
        )
        {
            pos_ += dir;
            length++;
        }

        if (length >= 3) vec.emplace_back(dir, length);
    }

    return vec;
}

bool CandyCrushGame::remove_matches(const bool is_in_setup)
{
    bool has_changed = false;

    std::vector will_be_destroyed(candies.get_width(), std::vector(candies.get_height(), false));

    auto activate_bombs = [&](const Position& pos)
    {
        will_be_destroyed[pos.x][pos.y] = true;

        for (const auto& dir : directions)
        {
            if (const Position pos_ = pos + dir;
                is_in_bounds(pos_) &&
                candies.get(pos_.x, pos_.y) != nullptr &&
                candies.get(pos_.x, pos_.y)->is_bomb())
                will_be_destroyed[pos_.x][pos_.y] = true;
        }
    };


    for (uint32_t x = 0; x < candies.get_width(); x++)
    {
        for (uint32_t y = 0; y < candies.get_height(); y++)
        {
            Position pos = { x, y };

            const auto res = check_surroundings(pos);
            if (res.empty()) continue;

            activate_bombs(pos);
            has_changed = true;

            for (auto& [dir_, length] : res)
            {
                for (uint32_t i = 1; i < length; i++)
                    activate_bombs(pos + dir_ * static_cast<int8_t>(i));

                if (!is_in_setup) refresh_score(score + length * (length + 5) + 10);
            }
        }
    }

    for (uint32_t x = 0; x < candies.get_width(); x++)
    {
        for (uint32_t y = 0; y < candies.get_height(); y++)
        {
            if (!will_be_destroyed[x][y] ||
                candies.get(x, y) == nullptr)
                continue;

            candies.destroy(x, y);
        }
    }

    return has_changed;
}

bool CandyCrushGame::bring_down()
{
    bool has_changed = false;

    for (uint32_t x = 0; x < candies.get_width(); x++)
    {
        for (uint32_t y = candies.get_height() - 1; y < candies.get_height(); y--)
        {
            const Position pos = { x, y };
            const Position up  = { x, y - 1 };

            if (y == 0)
            {
                if (candies.get(pos.x, pos.y) == nullptr)
                {
                    candies.set(
                        CandyMatrix::rand_uint(10) > 0
                            ? reinterpret_cast<CandyBase*>(new Candy(CandyMatrix::rand_color()))
                            : reinterpret_cast<CandyBase*>(new Bomb()),
                        pos.x, pos.y);

                    has_changed = true;
                }

                continue;
            }


            if (candies.get(pos.x, pos.y) != nullptr ||
                candies.get(up.x, up.y) == nullptr)
                continue;

            candies.swap(pos.x, pos.y, up.x, up.y);
            has_changed = true;
        }
    }

    send_cursor_to_top();

    return has_changed;
}


void CandyCrushGame::refresh(const bool is_in_setup)
{
    if (refreshing) return;
    refreshing = true;

    std::thread([this, is_in_setup]
    {
        do
        {
            do
            {
                if (!is_in_setup)
                    std::this_thread::sleep_for(200ms);
            }
            while (bring_down());
        }
        while (remove_matches(is_in_setup));

        send_cursor_to_top();
        refreshing = false;
    }).detach();
}

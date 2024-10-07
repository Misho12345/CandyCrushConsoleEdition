#pragma once
#include "CandyMatrix.h"

#include <array>
#include <variant>

#include <format>
#include <thread>
#include <mutex>


class CandyCrushGame
{
public:
    CandyCrushGame(uint32_t width, uint32_t height);
    ~CandyCrushGame();

    void run();

private:
    struct Direction
    {
        int8_t x;
        int8_t y;

        bool operator==(const Direction& other) const { return x == other.x && y == other.y; }

        Direction operator*(const int8_t scalar) const
        {
            return {
                static_cast<int8_t>(x * scalar),
                static_cast<int8_t>(y * scalar)
            };
        }
    };

    struct Position
    {
        int64_t x;
        int64_t y;

        bool operator==(const Position& other) const { return x == other.x && y == other.y; }

        Position operator+(const Direction& dir) const { return { x + dir.x, y + dir.y }; }
        Position operator-(const Direction& dir) const { return { x - dir.x, y - dir.y }; }

        void operator+=(const Direction& dir)
        {
            x += dir.x;
            y += dir.y;
        }

        void operator-=(const Direction& dir)
        {
            x -= dir.x;
            y -= dir.y;
        }
    };

    struct Match
    {
        Direction direction;
        uint32_t  length;
    };

    enum class InputKey
    {
        Enter,
        Space
    };


    void display_ui() const;
    void refresh_score(uint32_t new_score);

    void refresh_pixel(uint32_t x, uint32_t y) const;
    void refresh_pixel(const Position& pos) const;

    static void            send_cursor_to_top();
    [[nodiscard]] STRING_T get_quit_pos() const;

    void handle_movement(const Direction& dir);
    void move_cursor(const Position& pos);
    void swap(const Position& other_pos);

    [[nodiscard]] bool                                     is_in_bounds(const Position& pos) const;
    [[nodiscard]] static std::variant<Direction, InputKey> get_input();

    [[nodiscard]] std::vector<Match> check_matches(const Position& pos) const;
    [[nodiscard]] bool               remove_matches(bool is_in_setup = false);
    [[nodiscard]] bool               bring_down();


    void refresh(bool is_in_setup = false);


    static void printnl() { print('\n'); }

    static void print(const auto& value) { print(STRING("{}"), value); }
    static void printnl(const auto& value) { printnl(STRING("{}"), value); }

    template <typename... Args>
    static void print(const FORMAT_STRING_T<Args...> format, Args&&... args)
    {
        std::lock_guard lock(print_mutex);
        COUT << std::format(format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void printnl(const FORMAT_STRING_T<Args...> format, Args&&... args)
    {
        print(format, std::forward<Args>(args)...);
        COUT << std::endl;
    }

    static void flush()
    {
        std::lock_guard lock(flush_mutex);
        std::flush(COUT);
    }

    inline static std::mutex print_mutex;
    inline static std::mutex flush_mutex;


    CandyMatrix candies;

    Position cursor{ 0, 0 };
    bool     swap_mode  = false;
    bool     refreshing = false;

    uint32_t score            = 0;
    uint32_t display_score    = 0;
    bool     refreshing_score = false;

    static constexpr std::array directions =
    {
        Direction{ 1, 0 },  // Right
        Direction{ 0, 1 },  // Down
        Direction{ -1, 0 }, // Left
        Direction{ 0, -1 }  // Up
    };
};

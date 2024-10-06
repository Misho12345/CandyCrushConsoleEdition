#pragma once
#include "CandyMatrix.h"

#include <array>
#include <variant>

#include <iostream>
#include <format>
#include <thread>
#include <mutex>

template <typename T>
concept formattable = requires(T t)
{
    { std::format("{}", t) } -> std::same_as<std::string>;
};

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

    struct Surroundings
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

    static void                       send_cursor_to_top();
    [[nodiscard]] std::wstring        get_bottom_pos() const;

    void handle_movement(const Direction& dir);
    void move_cursor(const Position& pos);
    void swap(const Position& other_pos);

    [[nodiscard]] bool                                     is_in_bounds(const Position& pos) const;
    [[nodiscard]] static std::variant<Direction, InputKey> get_input();

    [[nodiscard]] std::vector<Surroundings> check_surroundings(const Position& pos) const;
    [[nodiscard]] bool                      remove_matches(bool is_in_setup = false);
    [[nodiscard]] bool                      bring_down();


    void refresh(bool is_in_setup = false);


    static void printnl() { print('\n'); }

    static void print(const formattable auto& value) { print(L"{}", value); }
    static void printnl(const formattable auto& value) { printnl(L"{}", value); }

    template <formattable... Args>
    static void print(const std::wformat_string<Args...> format, Args&&... args)
    {
        std::lock_guard lock(print_mutex);
        std::wcout << std::format(format, std::forward<Args>(args)...);
    }

    template <formattable... Args>
    static void printnl(const std::wformat_string<Args...> format, Args&&... args)
    {
        print(format, std::forward<Args>(args)...);
        std::wcout << std::endl;
    }

    static void flush()
    {
        std::lock_guard lock(flush_mutex);
        std::flush(std::wcout);
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

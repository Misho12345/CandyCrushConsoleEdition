#pragma once
#include <cstdint>
#include <array>

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

    [[nodiscard]] static Direction up() { return { 0, -1 }; }
    [[nodiscard]] static Direction down() { return { 0, 1 }; }
    [[nodiscard]] static Direction left() { return { -1, 0 }; }
    [[nodiscard]] static Direction right() { return { 1, 0 }; }

    [[nodiscard]] static std::array<Direction, 4> all()
    {
        return{
            up(),
            down(),
            left(),
            right()
        };
    }
};

struct Position
{
    uint32_t x;
    uint32_t y;

    [[nodiscard]] bool in_bounds(const uint32_t width, const uint32_t height) const { return x < width && y < height; }

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

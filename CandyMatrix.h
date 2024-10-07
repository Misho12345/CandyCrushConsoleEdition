#pragma once
#include <functional>
#include <vector>

#include "items/Candy.h"

class CandyMatrix
{
public:
    CandyMatrix(uint32_t width, uint32_t height, const std::function<void(const Position&)>& refresh);
    ~CandyMatrix();

    void destroy(const Position& pos);

    [[nodiscard]] bool remove_matches(uint32_t& score);
    [[nodiscard]] bool bring_down();

    void                set(Item* value, const Position& pos);;
    [[nodiscard]] Item* get(const Position& pos) const;
    void                swap(const Position& pos1, const Position& pos2);

    [[nodiscard]] uint32_t get_width() const { return width; }
    [[nodiscard]] uint32_t get_height() const { return height; }

private:
    std::vector<Item*>                   data;
    std::function<void(const Position&)> refresh;

    uint32_t width;
    uint32_t height;
};

#pragma once
#include "Item.h"

class Bomb final : public Item
{
public:
    void               destroy(CandyMatrix& matrix, const Position& pos) override;
    [[nodiscard]] bool on_move(CandyMatrix& matrix, const Position& pos, uint32_t& score) override;
    [[nodiscard]] bool destroy_on_move() const override { return true; }

    [[nodiscard]] STRING_T to_string() const override;
};

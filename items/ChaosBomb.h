#pragma once

#include "Item.h"

class ChaosBomb final : public Item
{
public:
    void destroy(CandyMatrix& matrix, const Position& pos) override;

    [[nodiscard]] bool     on_move(CandyMatrix& matrix, const Position& pos, uint32_t& score) override;
    [[nodiscard]] STRING_T to_string() const override;
    [[nodiscard]] bool     destroy_on_move() const override { return true; }
};

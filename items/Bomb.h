#pragma once
#include "Item.h"

class Bomb final : public Item
{
public:
    void destroy(CandyMatrix& matrix, const Position& pos) override;
    bool on_move(CandyMatrix& matrix, const Position& pos, uint32_t& score) override;

    [[nodiscard]] STRING_T to_string() const override;
};

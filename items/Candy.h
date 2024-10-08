#pragma once
#include "Item.h"

#include "../utils/Random.h"
#include <vector>

class Candy final : public Item
{
    enum class CandyColor
    {
        eRed,
        eYellow,
        eGreen,
        eBlue
    };

    struct Match
    {
        Direction direction;
        uint32_t  length;
    };

public:
    Candy() : color(static_cast<CandyColor>(Random::rand_u8(0, 3))) {}

    [[nodiscard]] static std::vector<Match> check_matches(const CandyMatrix& matrix, const Position& pos);

    [[nodiscard]] bool on_move(CandyMatrix& matrix, const Position& pos, uint32_t& score) override;
    [[nodiscard]] STRING_T to_string() const override;

    bool operator==(const Candy& other) const { return color == other.color; }

private:
    CandyColor color;
};

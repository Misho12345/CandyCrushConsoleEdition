#include "Bomb.h"
#include "../CandyMatrix.h"

void Bomb::destroy(CandyMatrix& matrix, const Position& pos)
{
    Item::destroy(matrix, pos);

    for (int8_t i = -1; i <= 1; i++)
    {
        for (int8_t j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0) continue;

            const Position pos_ = { pos.x + i, pos.y + j };
            if (!pos_.in_bounds(matrix.get_width(), matrix.get_height())) continue;

            if (matrix.get(pos_) != nullptr &&
                !matrix.get(pos_)->is_destroyed())
                matrix.destroy(pos_);
        }
    }
}

bool Bomb::on_move(CandyMatrix& matrix, const Position& pos, uint32_t& score)
{
    destroy(matrix, pos);
    score += 300;
    return true;
}

std::wstring Bomb::to_string() const
{
    #ifndef FORCE_ASCII
    return colored(128, 128, 128, STRING("ơ"));
    #else
        return colored(128, 128, 128, STRING("X"));
    #endif
}

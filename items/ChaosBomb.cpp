#include "ChaosBomb.h"

#include "../CandyMatrix.h"
#include "../utils/Random.h"

void ChaosBomb::destroy(CandyMatrix& matrix, const Position& pos)
{
    Item::destroy(matrix, pos);

    Position pos_{};

    do
    {
        pos_ = {
            Random::rand_u8(0, matrix.get_width() - 1),
            Random::rand_u8(0, matrix.get_height() - 1)
        };
    }
    while (pos_ == pos || matrix.get(pos_) == nullptr);

    matrix.destroy(pos_);
}

bool ChaosBomb::on_move(CandyMatrix& matrix, const Position& pos, uint32_t& score)
{
    destroy(matrix, pos);
    score += 200;
    return true;
}

STRING_T ChaosBomb::to_string() const
{
    #ifndef FORCE_ASCII
    return colored(230, 120, 30, STRING("§"));
    #else
    return colored(230, 120, 30, STRING("?"));
    #endif
}

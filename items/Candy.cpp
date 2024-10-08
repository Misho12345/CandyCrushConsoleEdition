#include "Candy.h"
#include "../CandyMatrix.h"

std::vector<Candy::Match> Candy::check_matches(const CandyMatrix& matrix, const Position& pos)
{
    std::vector<Match> vec;

    for (const auto& dir : Direction::all())
    {
        uint32_t length = 1;
        Position pos_   = pos + dir;

        if (!pos_.in_bounds(matrix.get_width(), matrix.get_height()) ||
            !(pos + dir * 3).in_bounds(matrix.get_width(), matrix.get_height()))
            continue;

        if (!instance_of<Candy>(matrix.get(pos))) continue;

        const auto candy = reinterpret_cast<Candy*>(matrix.get(pos));

        while (
            pos_.in_bounds(matrix.get_width(), matrix.get_height()) &&
            instance_of<Candy>(matrix.get(pos_)) &&
            *candy == *reinterpret_cast<Candy*>(matrix.get(pos_))
        )
        {
            pos_ += dir;
            length++;
        }

        if (length >= 3) vec.emplace_back(dir, length);
    }

    return vec;
}


bool Candy::on_move(CandyMatrix& matrix, const Position& pos, uint32_t& score)
{
    return matrix.remove_matches(score);
}

STRING_T Candy::to_string() const
{
    #ifndef FORCE_ASCII
    switch (color)
    {
        case CandyColor::eRed: return colored(238, 75, 43, STRING("■"));
        case CandyColor::eYellow: return colored(228, 208, 10, STRING("◆"));
        case CandyColor::eGreen: return colored(80, 200, 120, STRING("●"));
        case CandyColor::eBlue: return colored(60, 101, 221, STRING("▲"));
    }
    #else
        switch (color)
        {
            case CandyColor::eRed:    return colored(238, 75 , 43 , STRING("R"));
            case CandyColor::eYellow: return colored(228, 208, 10 , STRING("Y"));
            case CandyColor::eGreen:  return colored(80 , 200, 120, STRING("G"));
            case CandyColor::eBlue:   return colored(60 , 101, 221, STRING("B"));
        }
    #endif

    return STRING(" ");
}

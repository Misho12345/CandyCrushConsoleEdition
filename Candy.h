#pragma once
#include "CandyBase.h"

class Candy final : public CandyBase
{
public:
    enum class CandyColor
    {
        eRed,
        eYellow,
        eGreen,
        eBlue
    };

    explicit Candy(const CandyColor color) : color(color) {}

    void destroy(CandyMatrix& matrix, uint32_t x, uint32_t y) override { is_destroyed = true; }
    [[nodiscard]] bool is_bomb() const override { return false; }

    [[nodiscard]] STRING_T to_string() const override
    {
        #ifndef FORCE_ASCII
        switch (color)
        {
            case CandyColor::eRed:    return colored(238, 75 , 43 , STRING("■"));
            case CandyColor::eYellow: return colored(228, 208, 10 , STRING("◆"));
            case CandyColor::eGreen:  return colored(80 , 200, 120, STRING("●"));
            case CandyColor::eBlue:   return colored(60 , 101, 221, STRING("▲"));
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

    bool operator==(const Candy& other) const { return color == other.color; }

private:
    CandyColor color;
};

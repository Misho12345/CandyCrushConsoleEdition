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

    void destroy(CandyMatrix& matrix, uint32_t x, uint32_t y) override
    {
        is_destoyed = true;
    }


    [[nodiscard]] bool is_bomb() const override { return false; }

    [[nodiscard]] std::wstring to_string() const override
    {
        switch (color)
        {
            case CandyColor::eRed:    return colored(238, 75, 43, L"■");
            case CandyColor::eYellow: return colored(228, 208, 10, L"◆");
            case CandyColor::eGreen:  return colored(80, 200, 120, L"●");
            case CandyColor::eBlue:   return colored(60, 101, 221, L"▲");
        }

        return L" ";
    }

    bool operator==(const Candy& other) const { return color == other.color; }

private:
    CandyColor color;
};

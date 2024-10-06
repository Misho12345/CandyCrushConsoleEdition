#pragma once
#include "CandyBase.h"

class Bomb final : public CandyBase
{
public:
    void destroy(CandyMatrix& matrix, const uint32_t x, const uint32_t y) override
    {
        is_destoyed = true;

        for (int8_t i = -1; i <= 1; i++)
        {
            const uint32_t x_ = x + i;
            if (x_ >= matrix.get_width()) continue;

            for (int8_t j = -1; j <= 1; j++)
            {
                if (i == 0 && j == 0) continue;

                const uint32_t y_ = y + j;
                if (y_ >= matrix.get_height()) continue;

                if (matrix.get(x_, y_) != nullptr &&
                    !matrix.get(x_, y_)->is_destoyed)
                    matrix.destroy(x_, y_);
            }
        }
    }

    [[nodiscard]] std::wstring to_string() const override { return colored(128, 128, 128, L"⊛"); }
    [[nodiscard]] bool         is_bomb() const override { return true; }
};

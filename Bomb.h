#pragma once
#include "CandyBase.h"

class Bomb final : public CandyBase
{
public:
    void destroy(CandyMatrix& matrix, const uint32_t x, const uint32_t y) override
    {
        is_destroyed = true;

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
                    !matrix.get(x_, y_)->is_destroyed)
                    matrix.destroy(x_, y_);
            }
        }
    }

    [[nodiscard]] STRING_T to_string() const override
    {
        #ifndef FORCE_ASCII
        return colored(128, 128, 128, STRING("⊛"));
        #else
        return colored(128, 128, 128, STRING("X"));
        #endif
    }

    [[nodiscard]] bool is_bomb() const override { return true; }
};

#pragma once
#include <cstdint>

#include "macros.h"

class CandyMatrix;

inline STRING_T colored(const uint8_t r, const uint8_t g, const uint8_t b, const STRING_T& str)
{
    return STRING("\033[1;38;2;") +
            TO_STRING(r) + STRING(";") +
            TO_STRING(g) + STRING(";") +
            TO_STRING(b) + STRING("m") +
                str + STRING("\033[0m");
}


class CandyBase
{
public:
    virtual ~CandyBase() = default;

    virtual void destroy(CandyMatrix& matrix, uint32_t x, uint32_t y) = 0;

    [[nodiscard]] virtual STRING_T to_string() const = 0;
    [[nodiscard]] virtual bool     is_bomb() const = 0;

    bool is_destroyed = false;
};

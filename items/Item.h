#pragma once
#include "../utils/structs.h"
#include "../utils/macros.h"

class CandyMatrix;

inline STRING_T colored(const uint8_t r, const uint8_t g, const uint8_t b, const STRING_T& str)
{
    return STRING("\033[1;38;2;") +
            TO_STRING(r) + STRING(";") +
            TO_STRING(g) + STRING(";") +
            TO_STRING(b) + STRING("m") +
            str + STRING("\033[0m");
}


class Item
{
public:
    virtual ~Item() = default;

    virtual void               destroy(CandyMatrix& matrix, const Position& pos) { destroyed = true; }
    [[nodiscard]] virtual bool on_move(CandyMatrix& matrix, const Position& pos, uint32_t& score) = 0;

    [[nodiscard]] virtual STRING_T to_string() const = 0;

    [[nodiscard]] bool is_destroyed() const { return destroyed; }

private:
    bool destroyed = false;
};


template <typename T> requires std::derived_from<T, Item>
[[nodiscard]] bool instance_of(const Item* candy) { return dynamic_cast<const T*>(candy) != nullptr; }

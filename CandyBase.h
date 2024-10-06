#pragma once
#include <string>
#include <cstdint>

class CandyMatrix;

inline std::wstring colored(uint8_t r, uint8_t g, uint8_t b, const std::wstring& str)
{
    return L"\033[1;38;2;" +
         std::to_wstring(r) + L";" +
         std::to_wstring(g) + L";" +
         std::to_wstring(b) + L"m" + str + L"\033[0m";
}


class CandyBase
{
public:
    virtual ~CandyBase() = default;

    virtual void destroy(CandyMatrix& matrix, uint32_t x, uint32_t y) = 0;

    [[nodiscard]] virtual std::wstring to_string() const = 0;
    [[nodiscard]] virtual bool         is_bomb() const = 0;

    bool is_destoyed = false;
};

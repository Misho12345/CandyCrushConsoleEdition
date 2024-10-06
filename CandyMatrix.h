#pragma once
#include <functional>
#include <vector>
#include <random>

#include "Candy.h"

class CandyMatrix
{
public:
    CandyMatrix(const uint32_t width, const uint32_t height, const std::function<void(uint32_t, uint32_t)>& refresh)
        : data(width * height),
          refresh(refresh),
          width(width), height(height) {}

    void swap(const uint32_t x1, const uint32_t y1, const uint32_t x2, const uint32_t y2)
    {
        CandyBase* temp       = data[y1 * width + x1];
        data[y1 * width + x1] = data[y2 * width + x2];
        data[y2 * width + x2] = temp;

        refresh(x1, y1);
        refresh(x2, y2);
    }

    void destroy(const uint32_t x, const uint32_t y)
    {
        get(x, y)->destroy(*this, x, y);
        delete get(x, y);
        set(nullptr, x, y);
    }


    static uint8_t rand_uint(const uint8_t max)
    {
        static std::random_device       rd;
        static std::mt19937             gen(rd());
        std::uniform_int_distribution<> dis(0, max);

        return dis(gen);
    }

    static Candy::CandyColor rand_color() { return static_cast<Candy::CandyColor>(rand_uint(3)); }

    void set(CandyBase* value, const uint32_t x, const uint32_t y)
    {
        data[y * width + x] = value;
        refresh(x, y);
    };

    [[nodiscard]] CandyBase* get(const uint32_t x, const uint32_t y) const { return data[y * width + x]; }

    [[nodiscard]] uint32_t get_width() const { return width; }
    [[nodiscard]] uint32_t get_height() const { return height; }

private:
    std::vector<CandyBase*>                 data;
    std::function<void(uint32_t, uint32_t)> refresh;

    uint32_t width;
    uint32_t height;
};

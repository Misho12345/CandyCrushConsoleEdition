#include "CandyMatrix.h"
#include "items/Bomb.h"

CandyMatrix::CandyMatrix(
    const uint32_t width, const uint32_t height,
    const std::function<void(const Position&)>& refresh)
    : data(width * height), refresh(refresh),
      width(width), height(height)
{
    for (uint32_t x = 0; x < width; x++)
        for (uint32_t y = 0; y < height; y++)
            set(new Candy(), { x, y });
}

CandyMatrix::~CandyMatrix()
{
    for (const auto& item : data)
        delete item;
}



void CandyMatrix::destroy(const Position& pos)
{
    get(pos)->destroy(*this, pos);
    delete get(pos);
    set(nullptr, pos);
}


bool CandyMatrix::remove_matches(uint32_t& score)
{
    bool has_changed = false;

    std::vector will_be_destroyed(width, std::vector(height, false));

    for (uint32_t x = 0; x < width; x++)
    {
        for (uint32_t y = 0; y < height; y++)
        {
            Position pos = { x, y };

            const auto res = Candy::check_matches(*this, pos);
            if (res.empty()) continue;

            will_be_destroyed[x][y] = true;
            has_changed             = true;

            for (auto& [dir_, length] : res)
            {
                for (uint32_t i = 1; i < length; i++)
                {
                    const auto [x_, y_]       = pos + dir_ * static_cast<int8_t>(i);
                    will_be_destroyed[x_][y_] = true;
                }

                score += length * (length + 5) + 10;
            }
        }
    }

    if (!has_changed) return false;

    for (uint32_t x = 0; x < width; x++)
    {
        for (uint32_t y = 0; y < height; y++)
        {
            if (!will_be_destroyed[x][y] ||
                get({ x, y }) == nullptr)
                continue;

            destroy({ x, y });
        }
    }

    return true;
}

bool CandyMatrix::bring_down()
{
    bool has_changed = false;

    for (uint32_t x = 0; x < width; x++)
    {
        for (uint32_t y = height - 1; y < height; y--)
        {
            const Position pos = { x, y };
            const Position up  = { x, y - 1 };

            if (y == 0)
            {
                if (get(pos) == nullptr)
                {
                    set(
                        Random::rand_u8(0, 10) > 0
                            ? reinterpret_cast<Item*>(new Candy())
                            : reinterpret_cast<Item*>(new Bomb()),
                        pos);

                    has_changed = true;
                }
            }
            else if (get(pos) == nullptr && get(up) != nullptr)
            {
                swap(pos, up);
                has_changed = true;
            }
        }
    }

    return has_changed;
}


void CandyMatrix::set(Item* value, const Position& pos)
{
    data[pos.y * width + pos.x] = value;
    refresh(pos);
}

Item* CandyMatrix::get(const Position& pos) const { return data[pos.y * width + pos.x]; }

void CandyMatrix::swap(const Position& pos1, const Position& pos2)
{
    std::swap(data[pos1.y * width + pos1.x], data[pos2.y * width + pos2.x]);
    refresh(pos1);
    refresh(pos2);
}

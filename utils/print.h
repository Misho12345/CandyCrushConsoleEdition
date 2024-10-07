#pragma once
#include "macros.h"
#include <mutex>

template <typename T>
concept printable = requires(T value)
{
    { COUT << value } -> std::same_as<OSTREAM&>;
    { std::format(STRING("{}"), value) } -> std::same_as<STRING_T>;
};


inline std::mutex io_mutex;

void print(const printable auto& value)
{
    std::lock_guard lock(io_mutex);
    COUT << value;
}

void printnl(const printable auto& value)
{
    std::lock_guard lock(io_mutex);
    COUT << value << '\n';
}

template <printable... Args>
void print(const FORMAT_STRING_T<Args...> format, Args&&... args)
{
    std::lock_guard lock(io_mutex);
    COUT << std::format(format, std::forward<Args>(args)...);
}

template <printable... Args>
void printnl(const FORMAT_STRING_T<Args...> format, Args&&... args)
{
    std::lock_guard lock(io_mutex);
    COUT << std::format(format, std::forward<Args>(args)...) << '\n';
}

inline void printnl()
{
    std::lock_guard lock(io_mutex);
    COUT << '\n';
}

inline void flush()
{
    std::lock_guard lock(io_mutex);
    std::flush(COUT);
}
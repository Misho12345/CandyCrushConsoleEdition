#pragma once

#include <iostream>
#include <string>
#include <format>

// #define FORCE_ASCII

#if defined(__linux__) || defined(__APPLE__)
#define UNIX
#endif

#if !defined(UNIX) && !defined(_WIN32)
#error "Unsupported platform"
#endif

#if !defined(FORCE_ASCII) && !defined(UNIX)
#define FORCE_ASCII
#endif


#ifndef FORCE_ASCII
#define STRING_T std::wstring
#define TO_STRING(X) std::to_wstring(X)
#define STRING(S) L ## S

#define COUT std::wcout
#define FORMAT_STRING_T std::wformat_string
#define OSTREAM std::wostream

#else
#define STRING_T std::string
#define TO_STRING(X) std::to_string(X)
#define STRING(S) S

#define COUT std::cout
#define FORMAT_STRING_T std::format_string
#define OSTREAM std::ostream
#endif
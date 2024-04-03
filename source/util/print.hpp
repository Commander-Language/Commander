/**
 * @file print.hpp
 * @brief Contains print helpers
 *
 */

#ifndef PRINT_HPP
#define PRINT_HPP

#if defined(_MSC_VER)
#define WINDOWS_DEBUG
#endif

#include <iostream>
#ifndef WINDOWS_DEBUG
#include <ncurses.h>
#endif
#include <string>

namespace Util {

    extern bool usingNCurses;

    void print(const std::string& str);

    void println(const std::string& str);

}  // namespace Util

#endif  // PRINT_HPP

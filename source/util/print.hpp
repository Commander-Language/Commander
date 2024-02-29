/**
 * @file print.hpp
 * @brief Contains print helpers
 *
 */

#ifndef PRINT_HPP
#define PRINT_HPP

#include <iostream>
#include <ncurses.h>
#include <string>

namespace Util {

    extern bool usingNCurses;

    void print(const std::string& str);

    void println(const std::string& str);

}  // namespace Util

#endif  // PRINT_HPP

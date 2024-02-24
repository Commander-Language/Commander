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

    void print(std::string str);

    void println(std::string str);

}  // namespace Util

#endif  // PRINT_HPP

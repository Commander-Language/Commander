/**
 * @file scan.hpp
 * @brief Contains scan helpers
 *
 */

#ifndef SCAN_HPP
#define SCAN_HPP

#include "print.hpp"
#include <iostream>
#if defined(_MSC_VER)
#define WINDOWS_DEBUG
#endif
#ifndef WINDOWS_DEBUG
#include <ncurses.h>
#endif
#include <ncurses.h>
#include <string>

namespace Util {

    std::string scan(const std::string& str);
    std::string getstring();

}  // namespace Util

#endif  // PRINT_HPP

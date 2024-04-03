/**
 * @file print.cpp
 * @brief Contains the implementation of print.hpp
 *
 */

#include "print.hpp"

namespace Util {

    bool usingNCurses = false;

    void print(const std::string& str) {
        if (usingNCurses) {
            //printw("%s", str.c_str());
        } else {
            std::cout << str;
        }
    }

    void println(const std::string& str) {
        if (str.empty()) return;

        if (usingNCurses) {
            //printw("%s\n", str.c_str());
        } else {
            std::cout << str << '\n';
        }
    }

}  // namespace Util

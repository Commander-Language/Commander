/**
* @file print.cpp
* @brief Contains the implementation of print.hpp
*
 */

#include "print.hpp"

namespace Util {

    bool usingNCurses = false;

    void print(std::string str) {
        if (usingNCurses) {
            printw("%s", str.c_str());
        } else {
            std::cout << str;
        }
    }

    void println(std::string str) {
        if (usingNCurses) {
            printw("%s\n", str.c_str());
        } else {
            std::cout << str << std::endl;
        }
    }

}  // namespace Util

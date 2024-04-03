/**
 * @file scan.cpp
 * @brief Contains the implementation of scan.hpp
 *
 */

#include "scan.hpp"

namespace Util {

    std::string getstring() {
        // https://stackoverflow.com/questions/26920261/read-a-string-with-ncurses-in-c

        std::string input;

#ifndef WINDOWS_DEBUG
        // let the terminal do the line editing
        nocbreak();
        echo();

        // this reads from buffer after <ENTER>, not "raw"
        // so any backspacing etc. has already been taken care of
        int ch = getch();

        while (ch != '\n') {
            input.push_back(ch);
            ch = getch();
        }

        // restore your cbreak / echo settings here

#endif
        return input;
    }

    std::string scan(const std::string& str) {
        if (usingNCurses) {
            print(str);
            std::string input = getstring();
            return input;
        } else {
            std::cout << str;
            std::string input;
            std::cin >> input;
            return input;
        }
    }

}  // namespace Util

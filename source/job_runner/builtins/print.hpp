/**
 * @file print.hpp
 * @brief Defines the function used for print and println built-in commands
 *
 */
#ifndef PRINTBUILTIN_HPP
#define PRINTBUILTIN_HPP

#include "shared.hpp"

/**
 * @brief builtin print command
 * @param args - list of arguments
 * @param in - file descriptor to read from
 * @param out - file descriptor to write to
 * @details Depending on the first argument (i.e. print or println), adds a newline.
 */
ReturnInfo print(const List &args, int in, int out);

#endif

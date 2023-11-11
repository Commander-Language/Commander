/**
 * @file commander_exception.cpp
 * @brief Contains implementation for functions in commander_exception.hpp
 *
 */
#include "commander_exception.hpp"

namespace Util {
    const char* CommanderException::what() const noexcept { return errorMessage.c_str(); }
}  // namespace Util